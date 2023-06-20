
function main() {

  // Gather the used set oof tags
  const tagSet = new Set();
  const postList = [];
  const tagContainers = document.getElementsByClassName('sitemap_tag');
  for (let index = 0; index < tagContainers.length; index++) {
    const container = tagContainers[index];
    const pContainer = container.children[0];
    if (!pContainer) {
      continue;
    }

    const tagList = pContainer.textContent.split(',');
    tagList.forEach(tag => tagSet.add(tag));
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

  let numEnabled = tagSet.size;
  for (const tag of tagSet) {
    const tagElement = document.createElement('div');
    tagElement.className = "tag-filter-item";
    const tagElementLabel = document.createElement('span');
    tagElementLabel.innerHTML = tag;
    const tagElementButton = document.createElement('button');
    tagElement.append(tagElementLabel, tagElementButton);
    tagContainer.append(tagElement);


    // Whenever a tag is clicked, execute the filtering behavior
    tagElementButton.onclick = function() {
      // Handle enable/disable
      tagElement.remove();

      if (tagElement.classList.contains('disabled')) {
        tagElement.classList.remove('disabled');
        if (numEnabled === 0) {
          tagContainer.prepend(tagElement);
        }
        else {
          tagContainer.children[numEnabled - 1].after(tagElement);
        }
        numEnabled++;

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
        tagContainer.append(tagElement);
        numEnabled--;

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
