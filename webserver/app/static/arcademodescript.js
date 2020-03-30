"use strict";

function startGame(event) {
    document.getElementById("game").hidden = false;
    document.getElementById("startButton").hidden = true;
    socket.emit("startArcade", {});
}

function incrementLeftPlayerScore() {
    let score = parseInt(document.getElementById("leftPlayerScore").innerText, 10);
    score++;
    document.getElementById("leftPlayerScore").innerText = score;
}

function incrementRightPlayerScore() {
    let score = parseInt(document.getElementById("rightPlayerScore").innerText, 10);
    score++;
    document.getElementById("rightPlayerScore").innerText = score;
}

function configureWebSocket() {
    let socket = io.connect('http://192.168.7.2:5000/arcademode');
    socket.on('message', function (data) {
        switch (data) {
           /* case "getReady":
                document.getElementById("game").hidden = false;
                socket.emit('waitingForStart', "");
                break;*/
            case "start":
                socket.emit('waitingForClicks', "")
                document.getElementById("info").innerText = "GO!";
                document.getElementById("leftbulb").innerText = "left diode on";
                document.getElementById("rightbulb").innerText = "right diode on";
                document.getElementById("leftbulb").style.backgroundColor = "red";
                document.getElementById("rightbulb").style.backgroundColor = "red";
                break;
            case "0":
                socket.emit('waitingForClicks', "");
                incrementLeftPlayerScore();
                break;
            case "1":
                socket.emit('waitingForClicks', "");
                incrementRightPlayerScore();
                break;
            case "END":
                document.getElementById("game").hidden = true;
                let leftScore = +document.getElementById("leftPlayerScore").innerText;
                let rightScore = +document.getElementById("rightPlayerScore").innerText;
                let info;
                if (leftScore > rightScore) info = "Left player won with " + leftScore + " clicks in 5 seconds!";
                else if (leftScore == rightScore) info = "Players tied with " + leftScore + " clicks in 5 seconds!";
                else info = "Right player won with " + rightScore + " clicks in 5 seconds!";
                document.getElementById("info").innerText = info;
                document.getElementById('scoreSubmissionForm').hidden = false;
                break;
        }
    });
    return socket;
}

document.getElementById("startButton").addEventListener("click", startGame);
let socket = configureWebSocket();