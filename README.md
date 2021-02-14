# Counter-for-mobile-service-center.

This was a group mini project, a part of my acedamics. The main aim of this project was to get familiar in using services offered by uc/OS II.
The system would give a token number to the newly arrived customer and also if a window gets free then it will display the token number of the next customer to be served.

Services used: **Mailbox and Message Queue**

Mailbox was used to send the data(token number) from *status_check()* function to the *token_display()* function.
Message queue was used to store the token number of the newly arrived customer, since we need to diplay it on the LCD when a serive window is free. The message queue of **FIFO** type was used.
