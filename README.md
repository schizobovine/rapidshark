# RapidShark
Nerf RapidStrike CS-18 mod using an Arduino and LiPo battery upgrade to rain
foamy annoyance upon one's enemies.

The name originally came from my initial idea to use a pair of NSR Shark 40krpm
motors, originally for 12V slot car racing. It quickly became a reference to
Randall Munroe's [XKCD comic about lowering one's standards for success as a
project wears on...]<https://xkcd.com/349/>

# Versions

This project has undergone a number of major and minor redesigns, and my
documentation isn't exactly the best, especially for earlier versions where I
assumed this would be a quick hack involving relatively little electronics
knowledge (oh, to be 3 years younger and less naive...).

* Mark I
*  Completed build
*  Used slightly larger motors: 24mm 12V 12850rpm [PAN14EE12AA1]<http://www.digikey.com/product-detail/en/PAN14EE12AA1/P14346-ND/2417070>
*  Covered cut-outs for above with some horribly done epoxy work
*  4S LiPo battery (14.8V nominal)
*  Flywheels ran directly off battery voltage
*  Rate of fire controlled via diodes (1N5209's?) in series
*  Segmented LED display only showed battery voltage.
*  Extremely noisy, had jamming issues especially at higher ROF
*  Very clumsy controls (giant knob on the side for ROF)
*  Overall, kinda crappy prototype

* Mark II
*  Never finished build
*  Originally going to be 2S LiPo based
*  Proved too noisy for reasonable use

* Mark III
*  Completed build
*  Same [PAN14EE12AA1]<http://www.digikey.com/product-detail/en/PAN14EE12AA1/P14346-ND/2417070> as in Mark I
*  Used heat gun to deform plastic to accomodate motors
*  4S LiPo battery (14.8V nominal)
*  Flywheels again directly off line voltage
*  Rate of fire controlled via stupidly big linear regulator (LM335, then LT1083)
*  Custom PCB from OSHPark for speed controller
*  Actually bothered to paint this one
*  Slightly better slide switch for ROF control
*  Added pushbutton laser "sight" because reasons
*  First attempt at ammo counter using SparkFun Pro Mini 5V to drive a 2 digit segmented display
*  Detected shots at first using IR LED & photodiode, but was too noisy so went to reed switch
*  Button panel for fiddling with/resetting counter
*  Counter was awesome but proved very unreliable in practice
*  Noticable voltage drop-out during flywheel spin-up

* Mark IV
*  Work in progess
*  Skipping paint job to make this one a "sleeper" model
*  Motors unselected but will be 130 Mabuchi form-factor to fit in existing case with little modification
*  Will feature two proper H-bridges, the [Pololu VNH5019 Motor Driver Carrier]<https://www.pololu.com/product/1451>
*  Since the H-bridges will be electronically controlled, switched to subminiature microswitches hooked up to uC (instead of giant 15A microswitches directly flipping current around)
*  [Adafruit Metro Mini]<https://www.adafruit.com/products/2590> as main controller
*  OLED display (either SparkFun Microview or Ye Olde Generic From Ebay)
*  IR interrupter for dart firing detection confirmation, in combination with mechanical position detection for the dart pusher.
*  Whole slew of small custom PCB's to aid in wiring

# License

* AVR C code (basically, everything under the firmware directory) is licensed under the GPLv2.0.
* Everything else is Creative Commons 4.0, Attribution-Share-Sharealike (CC-4.0-BY-SA)

# Acknowledgements

I would not be nearly as far along (or in the ICU from exploding batteries) if
not for the help and previous work & knowledge of
[torukmakto4]<http://torukmakto4.blogspot.co.uk/>; read some of the linked
posts below for a good breakdown of the basic fire control circuit in the
RapidStrike.

I also would like to thank the folks from Blastersmiths UK for their
information shared on [Diaries of a Nerf
Armorurer]<http://nerfarmourer.tumblr.com>, as it both provided inspiration,
guidance, and a lot of helpful pictures for this project.

# References

[The Dart Zone: Wiring and electrical practices]<http://torukmakto4.blogspot.co.uk/2013/04/tech-wiring-and-electrical-practices.html>
[The Dart Zone: Standard Rapidstrike, an Illustrated Guide]<http://torukmakto4.blogspot.co.uk/2013/10/standard-rapidstrike-illustrated-guide.html>
[The Dart Zone: Why compensating for resistance using additional voltage is bad design]<http://torukmakto4.blogspot.co.uk/2014/05/why-compensating-for-resistance-using.html>
[Diaries of a Nerf Armourer: 'Sharked' Rapidstrike CS-18 - Raja and Triakis]<http://nerfarmourer.tumblr.com/post/59029888581/sharked-rapidstrike-cs-18-raja-and-triakis>
[Diaries of a Nerf Armourer: NSR Shark 40 Necrospy]<http://nerfarmourer.tumblr.com/post/69776618533/nsr-shark-40-necrospy>
[Diaries of a Nerf Armourer: Rapidstrike Modification Guide]<http://nerfarmourer.tumblr.com/post/57957334515/rapidstrike-modification-guide-also-commissions>
