/*
*
* Ferromagnetic RAM, i2c, multi device, 
* This driver needs only a callback for i2c read and write, and wil expose to the user
* a contiguous byte range. Read and writes will deploy optionally ecc, and can be 
* configured in such a way that writes are stripped or mirrored across the modules.
*/
