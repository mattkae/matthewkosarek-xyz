
function main() {
    var fullScreenButton = document.getElementById('full_screen_button'),
    resumeContainer = document.getElementById('resume');

    fullScreenButton.addEventListener('click', function() {
        resumeContainer.classList.toggle('resume_fullscreen');
    });
}


window.onload = main;
