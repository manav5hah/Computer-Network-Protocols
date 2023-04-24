# Computer Network Protocols

## Transmission Control Protocol (TCP)
### <u> Introduction</u>
TCP enables two hosts to establish a connection and exchange streams of data. TCP provides reliable, ordered, and error-checked delivery of a stream of TCP frames which are made of data packets. To establish a connection between the hosts, the server first initiates passive open in which it binds to and then must listen to a defined port for the client to
connect which is also known as active open. This process is known as a three-way handshake which is due to the exchange of three data-structures consisting of SYN, SYN-ACK, and ACK.

### <u>Implementation</u>
In this project, The TCP/IP protocol to send data packets in the network. The data packet is of 1024 bytes. <b>TCP frame</b> of <b>1036 bytes</b> is created of the following elements: 
<b>frame_type</b>, <b>sequence_number</b>, <b>acknowledgment</b>, and <b>packet</b>.

The <b>three-way handshake</b> is performed as follows:
After creating a socket, the server first binds to the newly created socket and listens to a defined post meanwhile the client creates a socket and tries to connect to the specified port.

## Additive Increase/Multiplicative           Decrease (AIMD)

### <u> Introduction</u>
In TCP protocol, AIMD is a <b>feedback control algorithm</b>. AIMD as its name suggests it increases the <b>congestion window(CWND)</b> linearly and decreases CWND exponentially on congestion detection.

### <u>Implementation</u>
On reaching the <b>slow start threshold (ssthresh)</b> this project then follows the AIMD and increases CWND linearly and halves the CWND on congestion detection which occurs at every <b>40th packet after the last congestion.</b>


## Slow Start and Fast Retransmit

### <u> Introduction</u>
<b>Slow start</b> is an algorithm used in TCP protocol for <b>congestion-avoidance</b>. It is mainly used for balancing the speed of the network. It slowly increases the number of TCP frames until it finds the optimal amount of carrying capacity.

<b>Fast retransmit</b> is another such algorithm used in TCP protocol for <b>congestion-avoidance</b>. On the drop of any packet, the previously received acknowledgment(ACK) is retransmitted consecutively for three times. On the sender side on detecting these duplicate acknowledgments, it retransmits the packet which was dropped.

### <u>Implementation</u>
Slow Start <b>doubles</b> the <b>congestion window(CWND)</b> on a round trip time at first. It increases the CWND by <b>1 Maximum Segment Size (MSS)</b> for every ACK received. Fast retransmit is implemented on congestion detection which occurs at <b>every 40th packet.</b>
