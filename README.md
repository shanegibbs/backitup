
```
sudo apt-get install -y \
  libdb6.0++-dev \
  libdb6.0++ \
  libprotobuf8 \
  libprotobuf-dev \
  protobuf-compiler
```

Update protocol buffers

```
protoc -I=src --cpp_out=src src/Database.proto
```

