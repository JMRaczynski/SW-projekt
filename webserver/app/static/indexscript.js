"use strict";

function startGame(event) {
    document.getElementById("game").hidden = false;
    document.getElementById("startButton").hidden = true;
    socket.emit('startClassic', {});
}

function toggle(number) {
    if (number === 0) {
        document.getElementById("leftbulb").style.backgroundColor = "red";
        document.getElementById("leftbulb").innerText = "Left diode on";
    }
    else {
        document.getElementById("rightbulb").style.backgroundColor = "red";
        document.getElementById("rightbulb").innerText = "Right diode on";
    }
}

function configureWebSocket() {
    let socket = io.connect('http://192.168.7.2:5000/');
    socket.on( 'connect', function() {
        socket.emit( 'my event', {
            data: 'User Connected'
        });
        //alert("POLACZONE");
    });
    socket.on('message', function (data) {
        //alert(data);
        switch (data) {
            case "toggle0":
                socket.emit('requestTime', {});
                toggle(0);
                break;
            case "toggle1":
                socket.emit('requestTime', {});
                toggle(1);
                break;
        }
    });
    socket.on('reactionTime', function (data) {
        //alert("WYNIKPOWINIENPRZYJSCHALO")
        let scoreInfo = document.getElementById('scoreInfo');
        scoreInfo.innerText = "You pressed the button after " + data + "ms.";
        if (counter < 4) {
            if (document.getElementById("leftbulb").style.backgroundColor == "red") {
                document.getElementById("leftbulb").style.backgroundColor = '555555';
                document.getElementById("leftbulb").innerText = "Left diode off";
            }
            else if (document.getElementById("rightbulb").style.backgroundColor == "red") {
                document.getElementById("rightbulb").style.backgroundColor = '555555';
                document.getElementById("rightbulb").innerText = "Right diode off";
            }
            socket.emit('nextRoundClassic', {});
            counter++;
        }
        else {
            socket.emit('requestResult', {});
            document.getElementById("leftbulb").hidden = true;
            document.getElementById("rightbulb").hidden = true;
        }
    });
    socket.on('score', function (data) {
        let scoreInfo = document.getElementById('scoreInfo');
        scoreInfo.innerText = "Your score is " + data + "ms."
        document.getElementById('scoreSubmissionForm').hidden = false;
    });
    return socket;
}

document.getElementById("startButton").addEventListener("click", startGame);
let socket = configureWebSocket();
let counter = 0;
