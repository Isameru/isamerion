
// -----------------------------------------------------------------------------------------------

const canvas = document.getElementById("canvas");

// Define the Module for the WebAssembly loader.
var Module = {canvas : canvas};

// -----------------------------------------------------------------------------------------------
// Canvas resizing
// -----------------------------------------------------------------------------------------------

// Sets the canvas size to fill the window.
function resizeCanvas() {
    canvas.width = window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
    canvas.height = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
}

// Call resizeCanvas() whenever the window is resized.
window.addEventListener('resize', resizeCanvas, false);

// Also call resizeCanvas() immediately to set the initial canvas size.
resizeCanvas();

// -----------------------------------------------------------------------------------------------
// Fullscreen and wake lock
// -----------------------------------------------------------------------------------------------

const fullscreenBtn = document.getElementById("fullscreen-btn");
let wakeLock = null;

// Releases the wake lock if it is active (while leaving fullscreen mode).
function releaseWakeLock() {
    if (wakeLock !== null) {
        wakeLock.release().then(() => {
            console.log("Wake lock released");
            wakeLock = null;
        });
    }
}

// Acquires a wake lock to keep the screen on while the app is in fullscreen mode.
async function requestWakeLock() {
    try {
        wakeLock = await navigator.wakeLock.request('screen');
        console.log("Wake lock active");

        document.addEventListener("visibilitychange", () => {
            if (wakeLock !== null && document.visibilityState === "visible" && document.fullscreenElement) {
                requestWakeLock();
            } else {
                releaseWakeLock();
            }
        });
    } catch (err) {
        console.error(`${err.name}: ${err.message}`);
    }
}

// Set up the fullscreen button.
fullscreenBtn.addEventListener("click", () => {
    // const elem = canvas;
    const elem = document.body;

    if (!document.fullscreenElement) {
        elem.requestFullscreen().catch(err => { alert(`Failed to enter fullscreen: ${err.message}`); });
        requestWakeLock();
    } else {
        document.exitFullscreen();
    }
});

// Release the wake lock when the user exits fullscreen mode.
document.addEventListener("fullscreenchange", () => {
    if (!document.fullscreenElement) {
        releaseWakeLock();
    }
});

// -----------------------------------------------------------------------------------------------
// "Options" button
// -----------------------------------------------------------------------------------------------

const optionsButton = document.getElementById('options-btn');
const gearIcon = document.getElementById('gear-icon');
let rotation = 0;

optionsButton.addEventListener('click', () => {
    rotation += 60;
    gearIcon.style.transform = `rotate(${rotation}deg)`;
});

// -----------------------------------------------------------------------------------------------
// "About" button
// -----------------------------------------------------------------------------------------------

const aboutButton = document.getElementById('about-btn');

aboutButton.addEventListener('click', () => {
    const aboutText = document.getElementById('about-box');
    if (aboutText.style.display === 'none' || aboutText.style.display === '') {
        aboutText.style.display = 'block';
    } else {
        aboutText.style.display = 'none';
    }
});
