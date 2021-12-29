
function main() {
    runCarousel();

    // // -- Note! We require a black background to properly see the WebGL, so we handle that here.
    // var themeElement = document.getElementById('theme_container');
    // var themeButtonList = document.querySelectorAll('#theme_selector > button');
    // themeButtonList.forEach(function(themeButton) {
    //     if (themeButton.id !== 'theme_button_default') {
    //         themeButton.addEventListener('click', function() {
    //             themeElement.style.background = 'black';  
    //         });
    //     }
    //     else {
    //         themeButton.addEventListener('click', function() {
    //             themeElement.style.background = 'white';  
    //         });
    //     }
    // });
}

function runCarousel() {
    var carouselContainer = document.getElementById("carousel"),
        imageContainer = document.getElementById('image_container'),
        leftButton = document.getElementById('carousel_left'),
        rightButton = document.getElementById('carousel_right');

    // Carousel logic
    var carouselPosition = 0,
        numImages = imageContainer.children.length;
    
    function onCarouselRight() {
        carouselPosition = (carouselPosition + 1);

        if (carouselPosition === numImages) {
            carouselPosition = 0;
        }
        updateCarousel();
    }
    
    function onCarouselLeft() {
        carouselPosition = (carouselPosition - 1);
        if (carouselPosition < 0) {
            carouselPosition = numImages - 1;
        }
        updateCarousel();
    }

    function onImageClicked() {
        var background = document.createElement('div');
        background.classList.add('carousel_image_expanded_container');
        var clone = this.cloneNode(true);
        clone.classList.add('expanded');
        background.append(clone);
        clone.addEventListener('click', function(event) { event.stopPropagation(); });
        background.addEventListener('click', function() { background.remove(); });
        document.body.parentElement.prepend(background);
    }

    function setImageClicked() {
        for (var i = 0; i < numImages; i++) {
            var image = imageList[i];
            image.addEventListener('click', onImageClicked);
        }
    }

    function updateCarousel() {
        var selectedChildPosition = -(carouselPosition * 240);
        imageContainer.style.transform = 'translate(' + selectedChildPosition + 'px, 0)';
        for (var i = 0; i < numImages; i++) {
            var image = imageList[i];
            if (i !== carouselPosition) {
                if (i === (carouselPosition - 1) % numImages) {
                    image.style.opacity = 0.3;
                } else if (i !== 0 && i === (carouselPosition + 1) % numImages) {
                    image.style.opacity = 0.3;
                } else {
                    image.style.opacity = 0;
                }
            } else {
                image.style.opacity = 1;
            }
        }

        rightButton.style.visibility = (carouselPosition === numImages - 1) ? 'hidden' : 'visible';
        leftButton.style.visibility = (carouselPosition === 0) ? 'hidden' : 'visible';
    }

    // -- Set up on image clicked
    var imageList = imageContainer.children,
        numImages = imageContainer.children.length;

    setImageClicked();
    updateCarousel();

    leftButton.addEventListener('click', onCarouselLeft);
    rightButton.addEventListener('click', onCarouselRight);
}

document.onload = main;
