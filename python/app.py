#!flask/bin/python
from flask import Flask, jsonify, request
from flask_cors import CORS, cross_origin
from passman import *

app = Flask(__name__)
CORS(app)
# cors = CORS(app, resources={r"/api/unlocked": {"origins": "*"}})
# cors = CORS(app, resources={r"/api/password": {"origins": "*"}})
cors = CORS(app, resources={r"/api/*": {"origins": "*"}})
app.config['CORS_HEADERS'] = 'Content-Type'

@app.route("/api/connected", methods=['GET'])
@cross_origin(origin='*', headers=['Content-Type', 'Authorization'])
def get_connected():
    return jsonify({'msg': passman_isConnected()})


@app.route("/api/toc", methods=['GET'])
@cross_origin(origin='*', headers=['Content-Type', 'Authorization'])
def get_toc():
    return jsonify(passman_toc())

@app.route("/api/unlocked", methods=['GET', 'OPTIONS'])
@cross_origin(origin='*', headers=['Content-Type', 'Authorization'])
def get_unlocked():
    return jsonify({'msg': passman_isUnlocked()})


@app.route("/api/password", methods=['POST', 'OPTIONS'])
@cross_origin(origin='*', headers=['Content-Type', 'Authorization'])
def post_password():
    json_data = request.get_json(force=True)
    # print(json_data)
    pw = json_data['password']
    # return True
    passman_attemptUnlock(pw)
    return jsonify(password=pw)


@app.route('/')
def index():
    return "Hello, World!"


if __name__ == '__main__':
    passman_create()
    app.run(debug=True)
