from bottle import route, run


@route("/")
def index():
    return {"hello": "world"}

if __name__ == "__main__":
    run(host="localhost", port=8000)
