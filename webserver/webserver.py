from app import app, db, socketio
from app.models import Score
from flask_socketio import SocketIO


@app.shell_context_processor
def make_shell_context():
    return {'db': db, 'Score': Score}


if __name__ == '__main__':
    socketio.run(app, host="192.168.7.2")
