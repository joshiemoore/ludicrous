# ludicrous
ludicrous is the fastest JSON API framework for Python in terms of single-process performance. Light speed was too slow, so we had to go straight to LUDICROUS SPEED!

ludicrous is not a general-purpose web framework. Rather, it is meant for creating fast JSON APIs for pairing with a frontend framework like React.

This library is currently very early in development, and should be considered unstable and unready for production. For the time being, the library and documentation are subject to breaking change with little notice.

ludicrous is free software licensed under the terms of the GNU General Public License Version 2.

## Quickstart

1. Install ludicrous from PyPI: `$ pip install ludicrous`

2. Copy the following code to `helloworld.py`:
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

3. Launch the server: `$ python3 helloworld.py`

4. Use your web browser to navigate to `http://localhost:8000/` and observe the JSON response.

See [docs/EXAMPLES.md](docs/EXAMPLES.md) for some examples to help get you started.

## Build from source
1. Clone the repository: `$ git clone --recursive https://github.com/joshiemoore/ludicrous.git`

2. Enter the directory: `$ cd ludicrous`

3. Build the library: `$ python3 -m build`

4. Install the library: `$ pip install .`

Note that ludicrous is intended for development and deployment on GNU/Linux and is not tested on any other operating systems.

## Performance
ludicrous is being developed around a custom JSON serializer which converts Python objects directly into JSON C strings.

Preliminary testing on my machine (12th Gen Intel Core i5-1240P) using [wrk](https://github.com/wg/wrk) shows that ludicrous is able to consistently sustain hundreds of thousands of requests per second.

Below is a selection of wrk results for endpoints returning `{"hello": "world"}` in a few frameworks. Each framework was tested 5 times, and the best of the 5 results is displayed here. The main point of interest is the `Requests/sec` field.

```
ludicrous:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.67ms   60.28ms   1.68s    99.41%
    Req/Sec    20.16k    14.15k  157.92k    64.57%
  7235771 requests in 30.10s, 662.45MB read
  Socket errors: connect 0, read 0, write 0, timeout 44
Requests/sec: 240390.46
Transfer/sec:     22.01MB

japronto:
Running 30s test @ http://localhost:8080/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.06ms  160.54us   6.50ms   93.71%
    Req/Sec    16.01k     1.42k   52.70k    95.76%
  5743105 requests in 30.08s, 569.61MB read
Requests/sec: 190899.12
Transfer/sec:     18.93MB

nodejs:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.02ms    2.16ms 100.90ms   99.46%
    Req/Sec     8.41k   596.33    23.19k    98.92%
  3018151 requests in 30.09s, 549.76MB read
Requests/sec: 100314.22
Transfer/sec:     18.27MB

fastapi
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    60.11ms    5.02ms 140.70ms   82.74%
    Req/Sec   551.43    154.96     1.32k    67.45%
  197824 requests in 30.10s, 26.79MB read
Requests/sec:   6572.38
Transfer/sec:      0.89MB

bottle:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.91ms   43.11ms   1.69s    99.57%
    Req/Sec     1.38k     0.94k    4.91k    63.57%
  99917 requests in 30.08s, 15.72MB read
  Socket errors: connect 0, read 0, write 0, timeout 39
Requests/sec:   3322.10
Transfer/sec:    535.31KB

flask:
Running 30s test @ http://localhost:5000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    77.99ms   77.18ms   1.77s    98.56%
    Req/Sec   156.98    104.71   640.00     58.42%
  54683 requests in 30.10s, 9.54MB read
  Socket errors: connect 0, read 31, write 0, timeout 41
Requests/sec:   1816.87
Transfer/sec:    324.70KB
```
