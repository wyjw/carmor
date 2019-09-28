The goal of our project is partitioning programs based on user annotations. 

Inputs are given by source code and annotations at the function granularity and for non-local variables, at instantiation time. The annotations mostly state on which machine a particular program is run. 

```
// @1
float addFunc(float x, float y) {
    return x + y;
}
// @2
float subFunc(float x, float y) {
    return x + y;
}
```

This means that the addFunc can only be called on machine 1 and subFunc can only be called on machine 2. Suppose that we had a main that wanted to do the following. 

```
int main () {
  float x = 6.0; //@1
  float y = 7.0; //@1
  x = addFunc(x,y)
  x = subFunc(x,y)
}
```
Then what we have is that this would be equivalent to having two binaries as follows.

```
// This is Machine 1.
int main () {
  float x = 6.0;
  float y = 7.0;
  x = addFunc(x,y);
}
```

```
// This is Machine 2.
int main () {
  rpc_call(x, ...); // This is a request for an element x from the other machine. This request could be rejected. 
  x = subFunc(x,y);
}
