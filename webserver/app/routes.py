from flask import render_template, flash, redirect, url_for, request
from flask_socketio import send, emit
from app import app, db, socketio
from app.forms import ScoreSubmissionForm
from app.models import Score
from random import randrange
from time import sleep, time
from threading import Thread
import Adafruit_BBIO.UART as UART
import serial

execute = True
UART.setup("UART1")
ser = serial.Serial(port="/dev/ttyO1", baudrate=9600)
ser.close()
ser.open()

score = 0
arcadeTime = 0
leftPlayerScore = 0
rightPlayerScore = 0


@app.route('/',  methods=['GET', 'POST'])
@app.route('/index',  methods=['GET', 'POST'])
def index():
    global score
    page = request.args.get('page', 1, type=int)
    scores = Score.query.filter_by(gameType="SR").order_by(Score.score.asc()).paginate(page, app.config['SCORES_PER_PAGE'], False)
    next_url = url_for('index', page=scores.next_num) \
        if scores.has_next else None
    prev_url = url_for('index', page=scores.prev_num) \
        if scores.has_prev else None

    form = ScoreSubmissionForm()
    if form.validate_on_submit():
        record = Score(username=form.username.data, score=score, gameType="SR")
        db.session.add(record)
        db.session.commit()
        flash('Score submitted for user {}'.format(form.username.data))
        return redirect(url_for('index'))
    return render_template('index.html', title='Home', form=form, scores=scores.items, next_url=next_url, prev_url=prev_url)


@socketio.on('startClassic', namespace='/')
def startClassicGame(message):
    print("KLASYCZNYTESTZACZETY")
    ser.write(bytearray("1", 'ascii'))
    msg = listenToArduino()
    send(msg)


@socketio.on('nextRoundClassic', namespace='/')
def nextClassicRound(message):
    print("KOLEJNA RUNDA")
    msg = listenToArduino()
    send(msg)


def listenToArduino():
    while ser.isOpen():
        print("OPENED")
        msg = ser.readline()
        msg = msg[:-2]
        msg = msg.decode("utf-8")
        break
    print(msg)
    return msg


@socketio.on('requestTime', namespace='/')
def getReactionTime(message):
    msg = listenToArduino() # Odebranie wiadomości od Arduino
    emit("reactionTime", msg)


@socketio.on('requestResult', namespace='/')
def getResult(message):
    global score
    score = listenToArduino()
    emit("score", score)


@app.route('/twoplayers',  methods=['GET', 'POST'])
def twoplayers():
    global score
    page = request.args.get('page', 1, type=int)
    scores = Score.query.filter_by(gameType="MR").order_by(Score.score.asc()).paginate(page, app.config['SCORES_PER_PAGE'], False)
    next_url = url_for('twoplayers', page=scores.next_num) \
        if scores.has_next else None
    prev_url = url_for('twoplayers', page=scores.prev_num) \
        if scores.has_prev else None

    form = ScoreSubmissionForm()
    if form.validate_on_submit():
        record = Score(username=form.username.data, score=score, gameType="MR")
        db.session.add(record)
        db.session.commit()
        flash('Score submitted for user {}'.format(form.username.data))
        return redirect(url_for('twoplayers'))
    return render_template('twoplayers.html', title='Two players mode', form=form, scores=scores.items, next_url=next_url, prev_url=prev_url)


@socketio.on('startMultiplayer', namespace='/twoplayers')
def startMultiplayerGame(message):
    print("MULTIPLAYERTESTZACZETY")
    ser.write(bytearray("2", 'ascii'))
    msg = listenToArduino()
    send(msg)

@socketio.on('nextRoundMultiplayer', namespace='/twoplayers')
def multiplayerNextRound(message):
    print("KOLEJNA RUNDA DLA 2 GRACZY")
    msg = listenToArduino()
    send(msg)
    print("wyslane")

@socketio.on('requestTime', namespace='/twoplayers')
def getReactionTime(message):
    msg = listenToArduino()
    emit("reactionTime", msg)


@socketio.on('requestResult', namespace='/twoplayers')
def getResult(message):
    global score
    score = listenToArduino()
    emit("score", score)


@app.route('/arcademode',  methods=['GET', 'POST'])
def arcademode():
    global score
    page = request.args.get('page', 1, type=int)
    scores = Score.query.filter_by(gameType="AM").order_by(Score.score.desc()).paginate(page, app.config['SCORES_PER_PAGE'], False)
    next_url = url_for('arcademode', page=scores.next_num) \
        if scores.has_next else None
    prev_url = url_for('arcademode', page=scores.prev_num) \
        if scores.has_prev else None

    form = ScoreSubmissionForm()
    if form.validate_on_submit():
        record = Score(username=form.username.data, score=score, gameType="AM")
        db.session.add(record)
        db.session.commit()
        flash('Score submitted for user {}'.format(form.username.data))
        return redirect(url_for('arcademode'))
    return render_template('arcademode.html', title='Arcade Mode', form=form, scores=scores.items, next_url=next_url, prev_url=prev_url)


@socketio.on('startArcade', namespace='/arcademode')
def startArcade(message):
    global score, leftPlayerScore, rightPlayerScore
    print("TRYBARKADEZACZETY")
    leftPlayerScore = 0
    rightPlayerScore = 0
    score = 0
    ser.write(bytearray("3", 'ascii'))
    msg = listenToArduino()
    send(msg)


"""@socketio.on('waitingForStart', namespace='/arcademode')
def sendStartSignal(message):
    global arcadeTime, leftPlayerScore, rightPlayerScore
    t = randrange(10000)
    sleep(t / 1000)
    send("start")
    arcadeTime = time()
    leftPlayerScore = 0
    rightPlayerScore = 0"""


@socketio.on('waitingForClicks', namespace='/arcademode')
def informAboutClick(message):
    global arcadeTime, score, leftPlayerScore, rightPlayerScore
    """t = randrange(50, 150, 1)
    player = randrange(2)
    sleep(t / 1000)
    if time() - arcadeTime > 10:
        send("END")
        score = max(leftPlayerScore, rightPlayerScore)
    else:
        send(str(player))
        if player == 0:
            leftPlayerScore += 1
        else:
            rightPlayerScore += 1"""
    print("KLIK")
    msg = listenToArduino()
    if msg == "0":
        leftPlayerScore += 1
    elif msg == "1":
        rightPlayerScore += 1
    elif msg == "END":
        print("KONCZYC")
        score = max(leftPlayerScore, rightPlayerScore)
    send(msg)



@socketio.on('my event', namespace='/')
def ddd(message):
    print("DDDD")

