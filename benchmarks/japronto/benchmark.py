from japronto.app import Application


def index(request):
    return request.Response(json={"hello": "world"})

if __name__ == "__main__":
    app = Application()
    router = app.router
    router.add_route("/", index)
    app.run()
