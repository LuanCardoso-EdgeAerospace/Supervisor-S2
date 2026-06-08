Driver for using the FRAM chips on I2C.

## API

```c
enum ERR_T read(u32 addr, u32 len, u8 *output);
enum ERR_T write(u32 addr, u32_len, u8 *output);
```

Existence or not of ECC should be completelly transparent, except
for the qualification of ERR_T:

```c
enum ERR_T{
    SUCCESS,
    SINGE_ERROR_CORRECTED,
    DOUBLE_ERROR_DETECTED,
    //Other errors
}
```

And an extra function that could be run with a low priority.

```c
ERR_T scrub(u32 addr);
```

It should be possible to dinamically drop some of the FRAM devices if
we detect that it has started to misbehave. 
> But moving data around and free it would need to keep track of the allocated pages.


## Interleaving

## Inline ECC

## Protocol for CPU access

```c
enum OPCODES{
    FRAM_READ,
    FRAM_WRITE,
    FRAM_STATUS
}
```