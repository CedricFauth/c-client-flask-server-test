# WARNING: This is a development server. 
# Do not use it in a production deployment.

from flask import Flask
from flask import request
from flask import jsonify
from markupsafe import escape

app = Flask(__name__)

@app.route('/<yourtext>',methods=['GET','POST'])
def hello_world(yourtext):
    return jsonify(req_text=escape(yourtext), req_method=request.method)

if __name__ == '__main__':
    app.run(host='0.0.0.0',debug=False)
