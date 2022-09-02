import ludicrous.route
import ludicrous.server


@ludicrous.route.route("/")
def index(request):
    return {"hello": "world"}

if __name__ == "__main__":
    ludicrous.server.run()
