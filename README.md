# simba_spectra

Converter from PCAP file with SIMBA Spectra messages to JSON lines.
Supported messages are OrderUpdate, OrderExecution, OrderBookSnapshot.


## How to build

Prerequisites:
* git
* cmake
* ninja
* c++26 compiler

```shell
git clone https://github.com/ortfero/simba_spectra
cd simba_spectra
mkdir build
cd build
cmake ..
cmake --build .
```


## How to use

```shell
simba_spectra <input-pcap-file> <output-jsonl-file>
```


## License

simba_spectra licensed under [MIT lisense](https://opensource.org/licenses/MIT).