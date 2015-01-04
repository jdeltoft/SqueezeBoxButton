


1. Install SqueezePlug from http://www.squeezeplug.eu/ (and make sure it's up to date)

      sudo apt-get update

2. Install cpanminus for PERL

      sudo apt-get install curl gcc-4.7
      sudo curl -L http://cpanmin.us | perl - --sudo App::cpanminus

3. Install Net::Telnet perl module

      sudo cpanm Net::Telnet

4. Install wiringPi from http://wiringpi.com/download-and-install/

5. Update $SBID (squeezebox mac address) in SqueezeBoxButtonCmds perl script, as well as $HOST to the IP of the
   Squeezebox server on your LAN.

6. For now I haven't made them daemons so just run them in the background each powerup of the Raspberry Pi

      shell_prompt> SqueezeBoxButton &
      shell_prompt> SqueezeBoxButtonCmds &

