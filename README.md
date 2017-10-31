# LCTF 2017: BeRealDriver

This problem is meant to design a LDW program which can be easily affected by modifying image input.

You can see `doc/design.pdf` to check the design pricenple of this problem.

Basically, the program will read a map image(which is XORed with a user input) representing the camera's input. Driving lanes will be found and saved as a input of LDW. Then the car input, which is also XORed with a user input, is checked via LDW to make sure the path is included in the driving lane. 

We designed several "crash areas", which represents the right path leading to a car "accident". A detector will check if all those areas are well contained in the driver's path.

Under normal mode, most of the crash areas will not be passed, and attack will generate a special map image, and a special operate file, in order to lead the car.

Files in `code/examples/road-cir-xor.jpg` and `code/examples/op-cir-xor.jpg` is one of the right solutions. Of course you can have some more interestring solutions, for example putting a rabbit on the map ;)

Flag is `LCTF{We1c0m3_to_ldw_wor1d_And_is_th3_malf0rmed_fi13_1337_en0ugh_f0r_me}`

The program is originally designed as a multi-thread w/ CUDA accelerated problem, but considering there is still no public available (and chep enough) GPU VPS (aka. Cloud server) for us, I have to abandon the idea.

This contest might be the last time for me, to write a challenge for CTF games. Thanks for XDSEC, and thanks for all my friends.

Happy hacking!

* * *

## Legal Statement

IDA, ATMEL, AVR and all other trademarks cited herein are the property of their respective owners.

Copyright of this repo is belonging to the original author of those code.

Unless otherwise stated, you are requested to follow GPLv3 to use code in this repo.

The following related parts are NOT allowed to use this code, unless a statement is signed and published by the author:

* Humensec (http://www.humensec.com)
* Network Behaviour Research Center (NBRC) in Xidian University (http://nbrc.xidian.edu.cn)
* School of Cyber Engineering of Xidian University (http://ce.xidian.edu.cn)
* Leaders, researchers, students and any other people or entity sharing common benefits with entities above

According to the relevant laws, including the *Cybersecurity Law of the People's Republic of China*, the author and other related entities will resort to legal measures if you are not complying this license.
