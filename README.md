<pre>
                 ___       __                __  _
                /   | ____/ /   _____  _____/ /_(_)_______  _____
               / /| |/ __  / | / / _ \/ ___/ __/ / ___/ _ \/ ___/
              / ___ / /_/ /| |/ /  __/ /  / /_/ (__  )  __/ /
             /_/  |_\__,_/ |___/\___/_/   \__/_/____/\___/_/
                                
                                version 1.2
                  Copyright (c) 2021-24 Elwynor Technologies

</pre>

## WHAT IS ADVERTISER?

 Advertiser allows you to display ads to your users while they are online. It
 can select ads which you have created and display them at regular intervals
 you configure. You control who views the ads and who doesn't see them; there
 is a key for your staff not to see the ads if you wish, and you can configure
 the ads so they are broadcast to only a selected audience. This audience can
 be selected to be only males, only females, only adults, only minors, to see
 an ad only once a day, to display only on a certain channel range, to display
 only during prime time hours you configure, and to be broadcast only to one
 random user selected at broadcast time. 
 
 Advertiser allows you to charge companies to sell their products and services
 to your users, and you can charge premium prices for ads broadcast during
 prime time, greatly increasing your profitability. Ads can be added, removed
 and configured while the system is running for maximum flexibility.

 
## INSTALLATION AND CONFIGURATION
 
 Simply unzip the archive to your Worldgroup server directory, add 
 it to your menu, configure the MSG file to your liking, and start 
 Worldgroup!  It's that easy! 
 
 In LEVEL3, Security and Accounting, option PSTKEY is the key that will prevent 
 users from seeing ads; this is useful so that you and your staff won't be 
 annoyed with the ads. Option EDITKEY is the key needed to list and edit ads. 
 Users with this key may list and change your ads.

 In LEVEL4, Configuration options, PSTSEC is how often in seconds to broadcast
 ads; so for example if you set this to 60, then once every minute an ad will
 be broadcast. This ad is chosen randomly from the list of ads that you
 currently have.

 Option DOBEEP controls whether or not users receive a "beep" at their
 computer with an ad.

 Option EDITBUG controls whether users in the Full Screen Editor (FSE) receive
 ads while in the editor.

 EDTNAM is the name of the Full Screen Editor module; this is included so that
 Advertiser will know which editor you are using in case this should change.

 VIEWSTG is the global command that you type to list your ads.

 EDITSTG is the global command that you type to insert, remove or edit ads.

 To insert an ad, using the default command, you would type: /adedit
 <pathname> <flags> where <pathname> is the path and file name of the text file
 of the ad, and flags are the flags you wish the ad to have. An example of
 pathname would be AD1.TXT or C:\ADS\AD5.TXT. The flags allowed are as follows;
 ONCE causes an ad to be displayed to any user only once per day, RANDOM causes
 the ad to be displayed to only one user chosen randomly, CHANNEL causes the ad
 to be displayed only on the channel range you specify between options FSTLINE
 and LSTLINE, MINOR displays the ad only to users under 18 years old, ADULT
 displays the ad only to users 18 years or older, MALE displays the ad only to
 males, FEMALE displays the ad only to females, and PRIME displays the ad only
 during the prime time hours you specify in options PRMSTR and PRMEND. More
 than one flag may be used in the same ad, or none at all.

 To remove an ad from being displayed, simply type: /adedit <pathname>

 To edit an existing ad, which would be to change its flags, simply type:
 /adedit <pathname> <flags> where flags is the new flags you wish the ad to
 have. The ad will then have the new flags settings you specify.

 FSTLINE is the starting channel number to use for flag CHANNEL, while LSTLINE
 is the ending channel number to use for this flag.

 PRMSTR is the starting time for prime time to use for flag PRIME, while
 PRMEND is the ending time for prime time to use for this flag.

 SHOWAUD controls whether ads displayed are listed in the Audit Trail.

 SHOWTEXT controls whether ads displayed are listed in the text file you
 specify with option FILNAM.
 
## TROUBLESHOOTING
 
 Some Audit Trail messages you might see begin with: ELW Advertiser Error. If
 you see a message like this, the line below it will tell what the problem is.

 Examples of these messages are: 
   Cannot get ad data file 
   Cannot open <filename> 
   <filename> too big 
 where <filename> is the name of the ad file.

 If you see these messages, you have a problem which needs to be corrected.
 
 The first message means the module can't find your ad in the database. There
 isn't much you can do about this and you should never see this message. 
 
 The second message means Advertiser can't find the text file you've specified
 for the ad, or else the module can't open the text file you've specified in
 option FILNAM for showing ads which have been displayed. You can tell which
 problem it is by the name of the path and file in <filename>; if it's the file
 name and extension you used in option FILNAM you know that this is the
 problem, otherwise you know that the problem is with an ad. You should verify
 that the path is correct and that the file exists.

 The last message means that the ad in the text file is too large (too many
 characters). You should reduce the size of the ad. The ads should generally be
 as small as possible, no more than one screen in size. The module can actually
 handle up to about 65k, but no ad should realistically ever even approach
 this.

## MODULE HISTORY
 
 Advertiser was written in September 1996 by "Easy Ed" Erdman of EzSoft. The
 module was available up through Worldgroup 3.0. EzSoft was acquired by
 AdventureComm, which was acquired by Elwynor Technologies in 2004.

 Elwynor Technologies ported the module to Worldgroup 3.2 in May 2021.
 In 2024, it was released for The Major BBS V10.
 
## LICENSE

 This project is licensed under the AGPL v3. Additional terms apply to 
 contributions and derivative projects. Please see the LICENSE file for 
 more details.

## CONTRIBUTING

 We welcome contributions from the community. By contributing, you agree to the
 terms outlined in the CONTRIBUTING file.

## CREATING A FORK

 If you create an entirely new project based on this work, it must be licensed 
 under the AGPL v3, assign all right, title, and interest, including all 
 copyrights, in and to your fork to Rick Hadsall and Elwynor Technologies, and 
 you must include the additional terms from the LICENSE file in your project's 
 LICENSE file.

## COMPILATION

 This is a Worldgroup 3.2 / Major BBS v10 module. It's compiled using Borland
 C/C++ 5.0 for Worldgroup 3.2. If you have a working Worldgroup 3.2 development
 kit, a simple "make -f ELWANNO" should do it! For Major BBS v10, import this
 project folder in the isv/ subtree of Visual Studio 2022, right click the
 project name and choose build! When ready to build for "release", ensure you
 are building for release.

## PACKING UP

 The DIST folder includes all of the items that should be packaged up in a 
 ELWANNO.ZIP. When unzipped in a Worldgroup 3.2 or Major BBS V10 installation 
 folder, it "installs" the module.
