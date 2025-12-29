
function main() {

  // Gather the used set of tags
  const tagSet = new Set();
  const postList = [];
  const tagContainers = document.getElementsByClassName('sitemap_tag');
  for (let index = 0; index < tagContainers.length; index++) {
    const container = tagContainers[index];
    const pContainer = container.children[0];
    if (!pContainer) {
      continue;
    }

    const tagText = pContainer.textContent.trim();
    const tagList = tagText.split(',').map(tag => tag.trim());

    // Replace the text content with styled tag badges
    pContainer.innerHTML = '';
    tagList.forEach(tag => {
      if (tag) {
        tagSet.add(tag);
        const tagBadge = document.createElement('span');
        tagBadge.className = 'post-tag';
        tagBadge.textContent = tag;
        pContainer.appendChild(tagBadge);
      }
    });

    postList.push({
      container: container.parentElement,
      tagList: tagList,
      enabled: tagList.length
    });
  }

  // Create the tag container
  const contentContainer = document.getElementById('content');
  const tagContainer = document.createElement('div');
  tagContainer.id = 'tag-filter-container';
  contentContainer.before(tagContainer);

  for (const tag of tagSet) {
    const tagElement = document.createElement('div');
    tagElement.className = "tag-filter-item";
    const tagElementLabel = document.createElement('span');
    tagElementLabel.innerHTML = tag;
    tagElement.append(tagElementLabel);
    tagContainer.append(tagElement);

    // Whenever a tag is clicked, execute the filtering behavior
    tagElement.onclick = function() {
      if (tagElement.classList.contains('disabled')) {
        tagElement.classList.remove('disabled');

        // Filter
        postList.forEach(post => {
          if (post.tagList.includes(tag)) {
            post.enabled++;

            if (post.enabled) {
              post.container.style.display = 'list-item';
            }
          }
        });
      }
      else {
        tagElement.classList.add('disabled');

        // Filter
        postList.forEach(post => {
          if (post.tagList.includes(tag)) {
            post.enabled--;
            if (!post.enabled) {
              post.container.style.display = 'none';
            }
          }
        });
      }
    };
  }
}

window.onload = main;
