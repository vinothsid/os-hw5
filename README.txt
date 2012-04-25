Assumptions
	Nr + Nw > N
	Nw > N/2
During PUT:
	Servers will lie only about version no. 
During GET:
	Servers will not lie about the version no.

For our lie detection algorithm to work the first condition is of prime importance.
Call the portion Nr+Nw-N as "OverLap", then our algorithm can detect <(OverLap)/2 liers (Note it is strictly lesser than), in case of servers 
conspiring to lie. =OverLap/2 liers can be detected when the lying servers tell different lies.

The Scheme:
This is best explained with an example. Let us assume 6 servers which are updated with a key k1 and value v1.
All of them have k1 v1 pair with version no. 1. Assume Nr =5 and Nw=6, and two of the servers will lie.
When a GET is performed lets assume only 5 servers reply, and two of them say k1 has value v1lie1 and v1lie2.
But 3 of the 5 lying servers also contain the updated value(as all 6 servers were needed to be updated for write to succeed Nw=6).
Now we can decide these two servers are lying by taking majority of the value among servers with the highest version number. 
So we can detect < 6+5-6/2 i.e upto 2 liers. Note if three were to lie, in this case the liers can form a majority and lie may go undetected.
But if all 3 tell a different lie we can detect all three of them as liers because the two servers telling the same truth will form majority.

Thus our scheme is based on predicting the true value based on number of servers telling the "same truth".

We extend the concept of majority in "same truth" to version numbers, where "same truth" is the value of the version number. We accept this 
version number to be the true version number of this key entry, increment this and echo this value to all the servers. Thus maintaining proper
version.
