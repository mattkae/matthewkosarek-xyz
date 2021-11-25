
function main() {
    runCarousel();
    runPosts();

    // -- Note! We require a black background to properly see the WebGL, so we handle that here.
    var bodyElement = document.querySelector('body');
    var themeButtonList = document.querySelectorAll('#theme_selector > button');
    themeButtonList.forEach(function(themeButton) {
        if (themeButton.id !== 'theme_button_default') {
            themeButton.addEventListener('click', function() {
                bodyElement.style.background = 'black';  
            });
        }
        else {
            themeButton.addEventListener('click', function() {
                bodyElement.style.background = 'white';  
            });
        }
    })
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
        
    }

    function updateCarousel() {
        var children = imageContainer.children,
            numChildren = imageContainer.children.length,
            selectedChildPosition = -(carouselPosition * 240);
        imageContainer.style.transform = 'translate(' + selectedChildPosition + 'px, 0)';
        for (var i = 0; i < children.length; i++) {
            var image = children[i];
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

    updateCarousel();

    leftButton.addEventListener('click', onCarouselLeft);
    rightButton.addEventListener('click', onCarouselRight);
}

function runPosts() {
    
}

main();
