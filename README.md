# carmor

## Goal of project.
The goal of our project is to partition programs based on annotations. This is usually the second part of a static analysis tool that has already decided on how to partition programs on a function granularity. This project actually does the partition of the program into different binaries, with remote procedural calls for communication between the binaries.

## Example. 
Inputs are given by source code and annotations at the function granularity and for non-local (or heap) ariables at instantiation time. The annotations mostly state on which machine a particular program is run. For this example, ```//@1``` refers to a function that has to run the first machine. 

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
  x = addFunc(x,y) //@1
  x = subFunc(x,y) //@2
  float *z = malloc(3 * sizeof(float)); //@1
  z[3] = 6.0; //@2
}
```
Then what we have is that this would be equivalent to having two binaries as follows. Notice that functions are now moved to each machine and they are run on a particular machine.

```
// @1
float addFunc(float x, float y) {
    return x + y;
}

// This is Machine 1.
int main () {
  float x = 6.0;
  float y = 7.0;
  x = addFunc(x,y);
  float *z = malloc(3 * sizeof(float)); //@1
}
```

```
// @2
float subFunc(float x, float y) {
    return x + y;
}

// This is Machine 2.
int main () {
  rpc_retrieve(x, ...); // This is a request for an element x from the other machine. This request could be rejected. 
  x = subFunc(x,y);
  rpc_modify(&z[3], ..., WRITE); // This is a request for the ability to write a value to a heap object on machine 1.
}
```
Note that all rpc calls can fail if the required communication is deemed not safe by either machine (like an unsecure machine asking for a secure heap array). 
