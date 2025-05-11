/*
    MIT License
    Copyright (c) 2025 Mariusz Łapiński

      ▄█     ▄████████    ▄████████    ▄▄▄▄███▄▄▄▄      ▄████████    ▄████████  ▄█   ▄██████▄  ███▄▄▄▄
      ███    ███    ███   ███    ███  ▄██▀▀▀███▀▀▀██▄   ███    ███   ███    ███ ███  ███    ███ ███▀▀▀██▄
      ███▌   ███    █▀    ███    ███  ███   ███   ███   ███    █▀    ███    ███ ███▌ ███    ███ ███   ███
      ███▌   ███          ███    ███  ███   ███   ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ ███▌ ███    ███ ███   ███
      ███▌ ▀███████████ ▀███████████  ███   ███   ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ███▌ ███    ███ ███   ███
      ███           ███   ███    ███  ███   ███   ███   ███    █▄  ▀███████████ ███  ███    ███ ███   ███
      ███     ▄█    ███   ███    ███  ███   ███   ███   ███    ███   ███    ███ ███  ███    ███ ███   ███
      █▀    ▄████████▀    ███    █▀    ▀█   ███   █▀    ██████████   ███    ███ █▀    ▀██████▀   ▀█   █▀
                                                                    ███    ███
*/

const canvas = document.getElementById("canvas");

// Load the WebAssembly module.
isamerionModule = null;
Isamerion({ canvas: canvas }).then((Module) => {
    console.log("Isamerion WASM module loaded");
    isamerionModule = Module;
});

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

// Check if the device is an Apple system (iOS or macOS), where wake lock and fullscreen are blocked for ordinary web pages.
function isAppleSystem() {
    if (navigator.userAgentData && navigator.userAgentData.platform) {
        const platform = navigator.userAgentData.platform;
        return platform.includes("Mac") || platform.includes("iPhone") || platform.includes("iPad") || platform.includes("iOS");
    } else {
        return /Mac|iPod|iPhone|iPad/.test(navigator.platform) || (navigator.userAgent.includes("Mac") && 'ontouchend' in document);
    }
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
// Canvas resizing
// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

// Sets the canvas size to fill the window.
function resizeCanvas() {
    canvas.width = window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
    canvas.height = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
}

// Call resizeCanvas() whenever the window is resized.
window.addEventListener('resize', resizeCanvas, false);

// Also call resizeCanvas() immediately to set the initial canvas size.
resizeCanvas();

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
// Fullscreen and wake lock
// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

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

if (isAppleSystem()) {
    // Hide the fullscreen button on Apple systems.
    fullscreenBtn.style.display = "none";
}
else {
    // Set up the fullscreen button.
    fullscreenBtn.addEventListener("click", () => {
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
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
// "Options" button
// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

const optionsButton = document.getElementById('options-btn');
const gearIcon = document.getElementById('gear-icon');
let rotation = 0;

optionsButton.addEventListener('click', () => {
    if (isamerionModule == null) return;
    isamerionModule.respawnScenario();
});

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
// "About" button
// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

const aboutButton = document.getElementById('about-btn');

aboutButton.addEventListener('click', () => {
    const aboutBox = document.getElementById('about-box');
    if (aboutBox.style.display === 'none' || aboutBox.style.display === '') {
        aboutBox.style.display = 'block';
    } else {
        aboutBox.style.display = 'none';
    }
});

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
