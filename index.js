
function main() {
    var themeSelector = document.getElementById('theme_selector');
    themeSelector.classList.remove('hidden');

    imageCallbacks();
}

function imageCallbacks() {
  const imageContainer = document.getElementById('image_container');
  
    function onImageClicked() {
        var background = document.createElement('div'); // 
        background.classList.add('image_item_expanded_container');
        var clone = this.cloneNode(true);
        clone.classList.add('expanded');
        background.append(clone);
        clone.addEventListener('click', function(event) { event.stopPropagation(); });
        background.addEventListener('click', function() { background.remove(); });
        window.addEventListener('keydown', function(e) { if (e.key === 'Escape') background.remove(); });
        document.body.parentElement.prepend(background);
    }

    function setImageClicked() {
        for (var i = 0; i < numImages; i++) {
            var image = imageList[i];
            image.addEventListener('click', onImageClicked);
        }
    } 

    // -- Set up on image clicked
    var imageList = imageContainer.children,
        numImages = imageContainer.children.length;

    setImageClicked();
}

main();
