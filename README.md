# ludicrous
ludicrous is the fastest Python web framework in terms of single-process performance. Light speed was too slow, so we had to go straight to LUDICROUS SPEED!

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
       return({"hello": "world"})

   if __name__ == "__main__":
       # host and port kwargs are optional
       ludicrous.server.run(host="localhost", port=8000)
   ```

3. Launch the server: `$ python3 helloworld.py`

4. Use your web browser to navigate to `http://localhost:8000/` and observe the JSON response.

## Build from source
1. Clone the repository: `$ git clone --recursive https://github.com/joshiemoore/ludicrous.git`

2. Enter the directory: `$ cd ludicrous`

3. Build the library: `$ python3 -m build`

4. Install the library: `$ pip install .`

Note that ludicrous is intended for development and deployment on GNU/Linux and is not tested on any other operating systems.

## Performance
ludicrous is being developed around a custom JSON serializer which converts Python objects directly into JSON C strings. We plan to develop ludicrous with a very fast core featureset implemented in C, along with additional opt-in modules that run slower but provide more features than the high-performance core. This plan will become clearer to understand as more features are added.

Preliminary testing on my machine (Ryzen 3 CPU) using [wrk](https://github.com/wg/wrk) shows that ludicrous is able to consistently sustain more than 100,000 requests per second. For simple responses like the hello world sample above, ludicrous is able to average around 130,000+ requests per second.

Below is a selection of wrk results for endpoints returning `{"hello": "world"}` in a few Python frameworks. Each framework was tested 5 times, and the best of the 5 results is displayed here. The main point of interest is the `Requests/sec` field.

```
ludicrous:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.75ms   53.42ms   1.67s    99.54%
    Req/Sec    12.36k     6.83k   71.83k    70.29%
  4057863 requests in 30.10s, 367.64MB read
  Socket errors: connect 0, read 0, write 0, timeout 48
Requests/sec: 134821.09
Transfer/sec:     12.21MB

django:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   256.98ms  117.68ms   1.99s    85.56%
    Req/Sec   105.86     53.15   474.00     67.24%
  35567 requests in 30.08s, 9.87MB read
  Socket errors: connect 0, read 0, write 0, timeout 83
Requests/sec:   1182.34
Transfer/sec:    336.02KB

flask:
Running 30s test @ http://localhost:5000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    95.06ms   61.33ms   1.78s    99.04%
    Req/Sec   130.37    108.99   535.00     55.53%
  42902 requests in 30.10s, 7.45MB read
  Socket errors: connect 0, read 59, write 0, timeout 27
Requests/sec:   1425.48
Transfer/sec:    253.36KB

fastapi:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   154.99ms   13.02ms 260.40ms   92.29%
    Req/Sec   223.91     98.65   690.00     49.58%
  76344 requests in 30.09s, 10.34MB read
Requests/sec:   2537.22
Transfer/sec:    351.84KB

bottle:
Running 30s test @ http://localhost:8000/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     5.08ms   59.83ms   1.80s    99.44%
    Req/Sec   635.17    520.33     2.59k    64.05%
  44549 requests in 30.09s, 6.97MB read
  Socket errors: connect 0, read 0, write 0, timeout 29
Requests/sec:   1480.37
Transfer/sec:    237.09KB

japronto:
Running 30s test @ http://localhost:8080/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     4.73ms  485.24us  37.59ms   95.00%
    Req/Sec     6.96k     1.09k   44.18k    96.17%
  2494095 requests in 30.09s, 247.37MB read
Requests/sec:  82884.96
Transfer/sec:      8.22MB
```
