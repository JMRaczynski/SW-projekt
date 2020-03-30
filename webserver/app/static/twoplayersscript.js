"use strict";

function startGame(event) {
    document.getElementById("game").hidden = false;
    document.getElementById("startButton").hidden = true;
    socket.emit("startMultiplayer", {});
}

function configureWebSocket() {
    let socket = io.connect('http://192.168.7.2:5000/twoplayers');
    socket.on( 'connect', function() {
        socket.emit( 'my event', {
            data: 'User Connected to multiplayer mode'
        });
    });
    socket.on('message', function (data) {
        if(data == "toggle") {
            socket.emit('requestTime', {});
            document.getElementById("leftbulb").style.backgroundColor = "red";
            document.getElementById("rightbulb").style.backgroundColor = "red";
            document.getElementById("leftbulb").innerHTML = "left diode on";
            document.getElementById("rightbulb").innerHTML = "right diode on";
        }
    });
    socket.on('reactionTime', function (data) {
        let scoreInfo = document.getElementById('info');
        let leftScore = document.getElementById("leftPlayerScore");
        let rightScore = document.getElementById("rightPlayerScore");
        if (data[0] === '0') {
            scoreInfo.innerText = "Left player pressed the button first - after " + data.substring(1) + "ms.";
            leftScore.innerText = +leftScore.innerText + 1 + "";
        }
        else {
            scoreInfo.innerText = "Right player pressed the button first - after " + data.substring(1) + "ms.";
            rightScore.innerText = +rightScore.innerText + 1 + "";
        }
        document.getElementById("leftbulb").style.backgroundColor = '555555';
        document.getElementById("rightbulb").style.backgroundColor = '555555';
        document.getElementById("rightbulb").innerHTML = "right diode off";
        document.getElementById("leftbulb").innerHTML = "left diode off";
        if (counter < 4) {
            socket.emit('nextRoundMultiplayer', {});
            counter++;
        }
        else {
            socket.emit('requestResult', {});
            document.getElementById("game").hidden = true;
        }
    });
    socket.on('score', function (data) {
        let scoreInfo = document.getElementById('info');
        let leftScore = document.getElementById("leftPlayerScore");
        let rightScore = document.getElementById("rightPlayerScore");
        if (+leftScore.innerText > 2) {
            scoreInfo.innerText = "Left player won the game! " + "He was faster " + leftScore.innerText + " times and had average reaction time of " + data + "ms.";
        }
        else scoreInfo.innerText = "Right player won the game! " + "He was faster " + rightScore.innerText + " times and had average reaction time of " + data + "ms.";
        document.getElementById('scoreSubmissionForm').hidden = false;
    });
    return socket;
}

document.getElementById("startButton").addEventListener("click", startGame);
let socket = configureWebSocket();
let counter = 0;
