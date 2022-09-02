# ludicrous examples

### Basic JSON GET
```
import ludicrous.route
import ludicrous.server

@ludicrous.route.route("/")
def hello(request):
    # objects returned from routes will be automatically
    # serialized into JSON responses by ludicrous
    return {"hello": "world"}

if __name__ == "__main__":
    # host and port kwargs are optional
    ludicrous.server.run(host="localhost", port=8000)
```

### POST data
```
import ludicrous.route
import ludicrous.server

@ludicrous.route.route("/")
def index(request):
    if request["POST"]:
        return {"result": request["POST"]}
    return {"result": "no post data!"}

if __name__ == "__main__":
    ludicrous.server.run()
```
