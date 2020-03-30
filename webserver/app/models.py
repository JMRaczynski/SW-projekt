from app import db


class Score(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(64))
    score = db.Column(db.Integer)
    gameType = db.Column(db.String(64))

    def __repr__(self):
        return '<User {}>'.format(self.username)