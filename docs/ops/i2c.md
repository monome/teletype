## Generic I2C

Generic I2C ops allow querying and sending commands to any I2C enabled devices connected to teletype. Before you can send or query you need to set the I2C address of the device using `II.A` (you might want to place that in your INIT script so that the address is set when you load a scene). 

You can send up to 3 additional parameters, which can be either byte values or full range teletype values (for something like velocity), which will get sent as 2 bytes (MSB followed by LSB). All parameters must be of the same type - if you need to send both byte and word values, use the bitshift ops to combine/split bytes. 

No validation or transformation is applied to any of the parameters - they are send as is. As dedicated ops are often 1-based, you might want to subtract 1 when reproducing them with the generic ops.

There are 2 sets of query ops - one for getting regular (word) values and one for getting byte values. If the address is not set, or if it's set but there are no follower devices listening at that address, query ops will return zero.
