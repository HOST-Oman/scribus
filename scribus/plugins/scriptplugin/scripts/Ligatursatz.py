#!/usr/bin/env python
# -*- coding: utf8 -*-

"""
    This file as a hole is published under the MIT license:

    ✂----------------------------------------------------------------------

    The MIT License (MIT)

    Copyright (c) 2013-2016 Ned Batchelder, Stephan Hennig, Werner Lemberg,
    Guenter Milde, Sander van Geloven, Georg Pfeiffer, Gisbert W. Selke,
    Tobias Wendorf, Lukas Sommer.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    ✂----------------------------------------------------------------------

    This file contains public domain code from Ned Batchelder:
    “Ned Batchelder, July 2007. This Python code is in the public domain.”
    See the Hyphenator class for details. Modifications from Lukas
    Sommer (2016), published under the MIT license.

    It contains patterns that are derived from the Trennmuster project,
    that publishes its files under the MIT license.
    Copyright (c) 2013-2014 Stephan Hennig, Werner Lemberg, Guenter Milde,
    Sander van Geloven, Georg Pfeiffer, Gisbert W. Selke, Tobias Wendorf.
    See the GermanLigatureSupport class for details.

    The rest of the code is from Lukas Sommer (2016), published under
    the MIT license.
"""

import re
import scribus

# This file is in UTF8. It uses also really quite some characters outside
# ASCII and even outside BMP. As sometimes the encoding makes problems
# (Scribus script console for example), we assert here that the encoding
# is interpreted correctly
if ord("a") != 97:
    raise RuntimeError("Source code encoding problem 01.")

if len("a") != 1:
    raise RuntimeError("Source code encoding problem 02.")

if ord(u"\xE4") != 0xE4:
    raise RuntimeError("Source code encoding problem 03.")

if len(u"\xE4") != 1:
    raise RuntimeError("Source code encoding problem 04.")

if ord(u"\u00E4") != 0xE4:
    raise RuntimeError("Source code encoding problem 05.")

if len(u"\u00E4") != 1:
    raise RuntimeError("Source code encoding problem 06.")

if ord(u"\U000000E4") != 0xE4:
    raise RuntimeError("Source code encoding problem 07.")

if len(u"\U000000E4") != 1:
    raise RuntimeError("Source code encoding problem 08.")

if ord(u"ä") != 0xE4:
    raise RuntimeError("Source code encoding problem 09.")

if len(u"ä") != 1:
    raise RuntimeError("Source code encoding problem 10.")

if ord(u"\u1E9E") != 0x1E9E:
    raise RuntimeError("Source code encoding problem 11.")

if len(u"\u1E9E") != 1:
    raise RuntimeError("Source code encoding problem 12.")

if ord(u"\U00001E9E") != 0x1E9E:
    raise RuntimeError("Source code encoding problem 13.")

if len(u"\U00001E9E") != 1:
    raise RuntimeError("Source code encoding problem 14.")

if ord(u"ẞ") != 0x1E9E:
    raise RuntimeError("Source code encoding problem 15.")

if len(u"ẞ") != 1:
    raise RuntimeError("Source code encoding problem 16.")

if ord(u"\U0001F404"[0]) != 0x1F404 and ord(u"\U0001F404"[0]) != 0xD83D:
    raise RuntimeError("Source code encoding problem 17.")

if len(u"\U0001F404") != 1 and len(u"\U0001F404") != 2:
    raise RuntimeError("Source code encoding problem 18.")

if ord(u"🐄"[0]) != 0x1F404 and ord(u"🐄"[0]) != 0xD83D:
    raise RuntimeError("Source code encoding problem 19.")

if len(u"🐄") != 1 and len(u"🐄") != 2:
    raise RuntimeError("Source code encoding problem 20.")


__version__ = '0.1'


class Hyphenator:
    """An implementation of Frank Liams hyphenation algorithm in
    Python.

    Original code by Ned Batchelder from
    http://nedbatchelder.com/code/modules/hyphenate.html
    Original license:
    “Ned Batchelder, July 2007. This Python code is in the public domain.”

    Differently as the original implementation, this implementation
    adds Unicode support. On the other hand, some other features
    of the original implementation are not available.
    """
    def __init__(self, patterns):
        """Precondition: “patterns” is of type “unicode”.
        Postcondition: Constructs a hyphenator object for the
        given patterns.
        """
        if type(patterns) is not unicode:
            raise TypeError("The “patterns” parameter must be of type "
                            "“unicode”, but it isn’t.")
        self.tree = {}
        for pattern in patterns.split():
            self._insert_pattern(pattern)

    #
    def _insert_pattern(self, pattern):
        # Convert the a pattern like 'a1bc3d4' into a string of chars 'abcd'
        # and a list of points [ 0, 1, 0, 3, 4 ].
        chars = re.sub(u'[0-9]', u'', pattern)
        points = [int(d or 0) for d in re.split(u"[^0-9]", pattern)]
        # Insert the pattern into the tree.  Each character finds a dict
        # another level down in the tree, and leaf nodes have the list of
        # points.
        t = self.tree
        for c in chars:
            if c not in t:
                t[c] = {}
            t = t[c]
        t[None] = points

    def hyphenate_word(self, word):
        """ Precondition: “word” is of type “unicode”.
            Postcondition: Given a word, returns a list of pieces of
            type “unicode”, broken at the possible
            hyphenation points. Note that patterns are typically
            lower-case-only, so you have to convert “word” to
            lower-case before calling this function (otherwise
            the word might get wrong hyphenation because the
            upper-case-letters are not recognized).
        """
        if type(word) is not unicode:
            raise TypeError("The word must have the data type “unicode”, "
                            "but it doesn’t.")
        else:
            work = '.' + word.lower() + '.'
            points = [0] * (len(work) + 1)
            for i in range(len(work)):
                t = self.tree
                for c in work[i:]:
                    if c in t:
                        t = t[c]
                        if None in t:
                            p = t[None]
                            for j in range(len(p)):
                                points[i + j] = max(points[i + j], p[j])
                    else:
                        break
            # No hyphens in the first two chars or the last two.
            # But we comment this code out because it is not
            # necessary for ligature setting.
            # points[1] = points[2] = points[-2] = points[-3] = 0
            # But it is necessary to do at least this, just to avoid empty
            # pieces. Otherwise, the pattern “von1” would lead for the
            # word “von” to the result “[u"von", u""]” which is not intented.
            points[1] = 0
            points[-2] = 0
        # Examine the points to build the pieces list.
        pieces = ['']
        for c, p in zip(word, points[2:]):
            pieces[-1] += c
            if p % 2:
                pieces.append('')
        return pieces


class GermanLigatureSupport:
    """Provides support for german ligature setting. The pattern is derived
    from the word list of the Trennmuster project. Also get_word_characters()
    is derived from the Trennmuster project. For more information about
    the Trennmsuter project: http://projekte.dante.de/Trennmuster

    License of the Trennmuster project:

    The MIT License (MIT)

    Copyright (c) 2013-2014 Stephan Hennig, Werner Lemberg, Guenter Milde,
    Sander van Geloven, Georg Pfeiffer, Gisbert W. Selke, Tobias Wendorf

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."""

    def __init__(self):
        return

    def simple_case_fold_for_lookup(self, my_unicode_string):
        """Before applying the hyphenation algorithm to a string, some
        “folding” has to be done. The german word “auffallend” has the
        ligature ["auf", "fallend"]. If it is the first
        word of a sentence, than it is written with capital letter
         “Auffallend”. The “case” (the
        fact that a letter is a small letter or a capital
        letter) does not matter. You can read
        more about this topic in the Unicode standard:
        3.13 Default Case Algorithms → Caseless matching
        The pattern uses lower case. So we have to map all
        upper case letters in a string to
        lower case letters before applying the
        hyphenation algorithm. Unicode describes
        “full case folding” and “simple case folding”.
        “full case folding” converts for example
        both lowercase ß and uppercase ẞ to ss: it
        maps one original character to two
        substitution characters. “simple case folding”
        leaves lowercase ß as is, and converts
        uppercase ẞ to lowercase ß. I think the
        only relevant application of this “one-to-many
        mapping” for the german language is the sharp
        s. As the patter is generated for
        both (normal german with ß and swiss
        german without ß but with ss), this “one-to-many
        folding” is not necessary. A simple
        toLowercase() with additional mapping
        of the lowercase long s (ſ) to the lowercase
        normal s should be enough.

        Preconditions: my_unicode_string is of type “unicode”.
        Postconditions: Returns a “unicode” value
        that corresponds to my_unicode_string, but has
        mapped uppercase characters to lowercase
        characters – or at least these that are important
        for our patterns. The mapping is guaranteed
        to be a one-to-one mapping of Unicode Scalar
        Values. That means that one Unicode Scalar
        Value is replaced by exactly one other
        Unicode Scalar Value. So the count of
        Unicode Scalar Values of the return value is equal
        to the count of Unicode Scalar Values of
        my_unicode_string. (Note that the count of code
        units might change between input and output
        if you do not use UTF32.)
        WARNING This function must be kept
        in synch with isWordCharacter().
        """
        if type(my_unicode_string) is not unicode:
            raise TypeError("The “my_unicode_string” parameter must be of "
                            "type “unicode”, but it isn’t.")
        return my_unicode_string.lower().replace("ſ", "s")

    __germanLigaturePatterns = (u"""
    .a4 .ab3b .abel2 .ab3l .ab1or .abs2 .ack2 .ada4 .ade2 .ad2r.af2 .ag2a .agen2
    .ag2r .ag2u .akro2 .al4 .ali4 .all2e .alt3s .alz2 .am4 .amt2s .an1 .and2 .ang4
    .angs2 .angst3 .ann2a .an3s .ans2p .anti5ge .an3z2 .ap1p .ap3po .ar4 .ark2
    .arm3ac .aro2 .art3ei .art5erh .arto2 .as4 .asbe2 .at4 .au2 .au4f3 .au4s
    .aus7sende .aus5tes .au4v .ä2 .äo2 .b4 .ba4 .bak4 .bau1s .be3erb .bel4f .ben2
    .bens2 .ber4 .berg3a .berg5eb .berg3r .be7ringe .bes4 .be2ve .bil4 .bi3na .bit1a
    .bo4 .bor4 .c2 .ca4 .ce2 .ch4 .d2 .da1l .dar3i .datei3 .de4b .de1i .de4los
    .de1mi .den2 .de1o .de2pa .de3ran .der4en .der2n .der4wi .de1s .de1te .dien4e
    .dien6st .dienst7ad .dio2 .dit2 .do2 .dorf1 .dro4 .e4 .eb2 .eben3d .ed4 .efer2
    .eg2 .ehe3 .ei4gen .ei4lau .eins4 .ei3sch .ei4sp .ei4s1t .ei2t2 .ei1z .el4
    .elb3s .elch3t .ele4 .emm2 .em4s .en2d .en3dem .end3er .en4d1r .en2f .en2g
    .enge2 .enn2 .enns1 .en2q .en2r .en2s .en2t3 .en4tr .er4dar .er4din .er1e .erf2
    .er3g .er6hart .er1i .er4lac .er8lang. .ern2 .er3na .er3neu .ers2 .er8stein
    .er1t .es4 .est2 .est4e .eß1 .et4 .ett4 .eu4g .eu1tr .ex1te .é2 .f4 .fa4 .fe4
    .fel4 .fer2 .fi4 .fie4 .fil4 .fin4 .g4 .ga4 .gd2 .gege2 .ge6het. .ge1hu .gel2
    .geld1 .ge3lu .ge3me .ge3nar .gen2e .gen4fe .gen2r .gen3te .ge1r4 .ge3re
    .ge7riere .ges2 .ge3u .gie4 .gin2 .go2 .guss1 .h4 .ha4 .haf2 .hal4 .han2 .hau4s
    .haut1 .he4 .hef2 .hel2 .hen2 .her6ber .her6bra .her5inn .her6man .her6rin
    .her4zi .hi4 .hin3u .ho4 .hom3b .hus4 .i4 .ide2 .ig2 .ih2 .il1 .ima4 .im4k .imm2
    .im3mi .im1pe .in3dem .in3des .indi5ge .in3e .in1g2 .inge2 .in4geb .ink2 .in4sel
    .in3ser .in1z .irm2 .is2 .isch2 .is4f .it2 .k4 .ka2 .kal2 .kan4 .kat2 .kat3io
    .kat4m .ke4 .kel2 .ken2 .ker4 .ki4 .ko4g .ko1i .kom1 .kon8trolle. .ko4s .kose3
    .ku4b .kus2 .l4 .la4 .lan2 .lau4s .lär2 .le4 .lei4 .leich4 .len2 .ler2 .li4
    .licht7ers .lin2 .lit4 .lo4 .log3in .lor2 .los3s .lub2 .lut2 .ly4 .m4 .ma4
    .mal4e .man2 .mas4 .me4 .mee2 .mel4 .men2 .mer4 .mi4 .min2 .mo4 .mor4 .ms4
    .mund3a .mus2 .n4 .na2 .nar4 .näs3c .ne4 .nen2 .ner2 .ni4 .nie4 .nin2 .no4 .nus2
    .o4 .ob1a .obe4 .oben3 .ob1l .of3fer .oh4 .on2k .onto8logis .or4 .oran2 .ort2
    .orts3e .os2 .ost6alg .ost5end .ost3h .ost1r .ot2 .ot3a .ot4f .ot4m .ott4 .ox4
    .öd2 .öl3b .öl1l .öl1z .p4 .pa4 .par4k .par4l .par4m .pat4 .pel2 .pen2 .per4l
    .pf4 .ph2 .pi2 .pier4 .po4 .por4 .ps2 .r4 .ra4 .raf4 .ran2 .rang5te .ranzu4
    .re1b .reb3s .re3cha .rei4 .rem4 .re4mag .re1mi .ren2 .re1q .re3tri .ri4 .rif4
    .rik4 .ro4 .rom2 .rom4a .ror2 .rös1c .ruhr1 .run2 .s4 .sa4 .sab4 .sal4 .san2
    .sch4 .schen4 .se4 .sel2 .sen2 .ser4 .sie4 .sig2 .sim2 .so3ge .son2 .st4 .stan4
    .ste4 .sten2 .ster4 .t4 .ta4 .tab2 .tage4s3 .tal3la .tan4 .tank3l .tat3ta .tat1u
    .te4 .tei2 .ten2 .ter4 .tes4 .test3r .ti4 .tid1 .til2 .tit2 .to4 .tör3c .tre4
    .trie4 .tro2 .ts4 .tur4 .ty2 .u4 .uer2 .uh2 .ul2 .ull2 .um3 .un1a .un1d .un1g
    .un1k .un1n .un3s .un1z .ur1 .ur3b .ur2i .urin3t .ur3om .ur3t .ur3z .us2 .ut2
    .v2 .va4 .val2 .vel2 .ven2 .ver3be .ver3d .ver3g .ver3t .ver5v .vil2 .w6 .walt2
    .weg3 .wei4 .wen4 .wer4b .wer4k .wider7tes .wie3f .wil2 .wo2h .wort5en .x2 .y2
    .yo2 .z2 .za4 .ze2 .zer1n .zin4 .zo2 4a. aa2 a1ab a1ace aa3do aal1di aal1t a1an
    aan1g 2aar aar3a aar3b aar3k2 aar3n aar3r aar3s aar3ta aar3th aar3tr aar3z aat3h
    aat1r aat4s1 2a3au a1ä a2b 2aba4 ab2am a4b1ar4 ab1auf ab1ä ab2äu ab4bas 1abbi
    ab2c 1abd 2abe. ab1eb 2a3bec abe1e ab1eil 2a4be2l abe3lis a4ben aben1z ab1erk
    aber2n ab1er4r ab1erz ab3esse ab1eß 2a3bet a5betr 2abew 1abf 1abg 3abga 3abh
    2abi ab1ins ab1ir abi3st ab1it ab3itur 1abko ab1l 1abla ab3lag 1ablä 2able ab2lo
    3a4b3lö 1abn ab2of a4bon 2abor a3bos abo3se a3boß 4abot ab1r ab3rec 2abrö 2abru
    2abrü 1abs 2abs. 2absar ab3s2i ab3s2p abs2t2 2abst. ab3ste ab3stu ab1t 1abtei
    2abu abu3b ab3ur3 ab3us 2abü 1abw 2aby 1ab3z 2ac. 2aca 2acc 2ach. ach1a ach3au
    2achb ach6bars 2ache ach1ec ach1ei ach3erh ach3erl ach3erw 4achf ach1l ach3lö
    ach3m ach1n ach1ob ach1ö 2ach1r 6achsens achs2i ach3sin ach3spr ach4tak
    acht7ersc ach2t1o ach1uf ach3ü 2achv 4ach1w ach3z a4ci 4ack. acke2s ack2mu
    ackmus3 ack1n ack3sl ack3sta 2acl a2co 2a1cu a2d 4ad. ada2 2ada. ad2ag ad1ama
    ad1an 3ad3ap adar3 ada4v 1adä ad1c ad2dis 2ade. 2adeg ad1ein ade3k ade2l 2aden
    aden3ti ade1ra ade1ro 4ades2 ade3sp ade3str 2adf 2adh 4adi adie4 a3dikt adi4p
    ad2ke ad3len ad3me ad3na ad2ne ad2ob a4dol 2adp 2adq ad1rah 2ad1rec ad1rei
    a3drom ad1run 2ads2 ad2se ad3st ad3sz 2ad2t1 adt3an adt3r ad4vo 2ae4 a1eb a1ed
    a1ei ael2s a3em aeo3 a1ep a1erh 3aer2o1 aes2 a1ex af1ab a2f3an a2f1au 2afe af1ec
    af2f af3fam aff3ei af3fr af4fro 2afi 2af1l a5flu 2afo a4foh af1rau af1rä af1re
    af1rö af1ru af3s2a af1s2h af2s1p afs2t af3ste 2af2t aft1a aft1r aft5re af3ur
    af3z a2g 4ag. 2aga ag1ab ag1ad ag1am ag1ar ag1au aga2v a3gä ag2d1 2age. age1b
    age1g age2n ag2er 2ages a4ge2sa age2s3p age4spe ag3esse age4s3ti a4get 1ag3gr
    ag4gu 2ag1l ag2lan ag2ma ag1na ag2nat agner4s ag2ni 1agoge 3agogie a3gons a4g1re
    a4g1ri a5gru ag1rum ag3s2ah ags4eid ag3sta ag3ste 2agt 2ah. 2aha a1hä ahdi3 2ahe
    ahe1g ahe3in ahe1l ah1erh a1hi ah2j ah2l ahl3a ahl1ei ahl5ente ahl3erd ahl3erh
    ahl1sz ah4m ah4n ahn1a ahner4e aho2 a4hor ah1os a2h1ö ahr1a ahr2e ahr3erl ahre4s
    ahr4tri 2ahs ah1se ah2t3r aht3se aht3sp aht1z ah1wä a3hy ah3z 2a2i ai1a4 aian1
    aib1l ai1bu aid2s aids1t aien3 ai1f aig2 ai1gl ai2j ai3k2r ai3ku ai1la ai1lä
    ail3lis ail1tr aim2 ai1ma ai2man ai1mo ain2a ai3nac a3ind ain4e ain3f a3in1g
    ain2m ain2r ain3sp 3airb air5l ai3schw ais2e2 ait2 ai1ta ai2w aje4 a2k 2ak.
    2akal 2a3kam 2akar ak4at 1akaz a3kä 2akb 2akc 2akd 2ake aket3z ak3f 2aki a3kil
    ak2ka 4akl 2ako ako3k ako3l 2akr 4akra ak1rau 4akrä ak1res a4kro 2aks ak3sc
    ak1sh ak1so 2akta akt2an 2aktb ak2te akt2er 2aktik akt1r 2aktsi 2aktsp 2aktst
    a4kum 2akun a3kur 4a3kü 1akz ak3ze 4al. 2ala. al1ab alach2 al1af al1age a2lai
    al1am al1ana al1ang al1ans al3anz 2al3arr al1asi al1ass 2alat alat3s al1au
    al3auf al3aug al1äm alb3erw alb1l alb3li alb1ru alb3st al1c al2dac alde4 al2dr
    4ale al1eb al1ec ale4d al3ef al1eh al1ei a1leic al3ein a1lek al1el a3lenas
    al3ends al1epo al1erb al1erf al1erh al3erl al1ert ale4s al1esk ale3ta al1eth
    al1eu al1exi al3fe al3gas al2gli 1algo al3h a1lief a1l2imb al1ins alin4sc a2lis
    alk1le 1alkoh al3kre alk3s alk1se al3kür all1ab all3ar all5aufb al1läu alle4g
    al2ler all5erfa 4allin al1lip al1lit al1loh al4lok all3öse al2lu alm3ast al2me
    al3n al1ob 3aloe 4alog alo6gene al1ope al1orc a2l1ö 3alpe. al3per 3alph 2al3pr
    al3r 2als. al1ska als2te al5sterb al3su al4sum al2tak alt3alg al2tat alt3eig
    alt3erf 4alth alt1op alt3reu alt3ric alt2se alt4stü al1tur al2tü 2alty a2lu
    alu3f alu3g al1umb al3ur alut3s 4aly2 2alz al2zw am2 2am. 2ama. a1mac ama3k
    a1mal. a1mals 2aman2 a4mane a2mas a1max 1amaz 2amä am3d ame2 2ame. 2amel am4en
    amen2d amens3e amer2a am3erf amer2u 2a1mir 2amis a3miss 2amit ami3t2a 2am3l am4m
    2amm. 2ammal ammer6li 4amml 2ammt amo2 a2mol a2mö amp2 2ampe. 2ampen amper2
    amp4f ampf1a 2ampo am4pr am3ra am3rö ams2 am3sp am3str 1amt. amt1a amt1ä amt3ern
    amt1r 2amu am3unt am4wa a2n 2an. an2a 2anab an3abb ana1c an3alg 2anam anama3r
    2anan an3ang ana3po an3ar an4are an3ath an4atm an3au 1anb 2anbu 2an2c 4and.
    an3d2ac an2de and3ei ande2l ande4s andes3s 2andi an2dor an4drom and1rü and4sas
    and6spas and6s5paß an2du and1ur an4dy 2ane an3ec a4nei an2ei. a4nel a4nen a4ner
    a4nes an1eur anex1 1anfä 2anfi anf3la anf3rau anf2t anf2u 4ang. an2g1ar an3gau
    3angeb ang1ei ang3erf ang3erz 2angf 2angh 2angie ang1la ang1lä ang1n 2an2go
    ang1ra angre3g 1angri 4angs. ang3str ang1th an6halt. an8halts. an4haus 2ani
    4anie ani2l 3anim anis2 an2it a3niv 2an2j 2ank. ank1ak ank1an an2ke an3ket
    ank3ind ank1no ank1ra ank1rä 2anks ank1se 2ankt2 1anl 1anmu 2ann 3anna an2nas
    3annä ann2e an2net ann2g an2nie an2no an3noc ann4sto a4nom an1or 2anö 1an3r
    2anrö 4ans. 1ansc ans4ga 2ansk an1skr ans1pa an3sta ans2te an3stei an3str an1s2z
    4ant. an2tag ant3ar an2tau an2tä an3tät 2ante. 1antei 2antem an2ten 3antenn
    2anter 2antes ant2g an2th an3thei 1anthr an2ti ant2l an2to anton2 3antr 4ant2s
    ant3st 1antw 2anu 2anv 1anw 2anz 4anz. 1anzah 3anzei an2zen anz1in 4anzl 3anzu
    3anzü an2zwa an2zwi 2ao ao1b ao2l ao1m a1op a1or2 ao2s ao3ts a1ö 2ap. 2apa a1pä
    2a2pe ap2f 1apfel a1pfl a1phr a2pi 2a2pl apo1d apo1k apo2l apo1s ap2po ap2pu
    2apr a1prä ap4sc ap4so apt4 ap3to 2apu ar2 2ar. 2ara ar3abf ar3abt arad2 arag2
    ar3al ar3ang ar3ans ara3nu ar3anz ara1p ar3ap1p ar3ar a2ras ar3au ara1ve ar3är
    2arb. 2ar4ba arb1au arb3eie arb3eim 2arbek 2arben ar3ber 4ar3bes ar3bet 2arbi
    ar3bis 2arb1l 2arbr 2arb3s2 2arbt arb1th 2arbu arb3un 1archi arch2t 2ard ar3dam
    ar3dar ar3deb arden1 ar3don ard1r 2are are1b a1rec ar3eff are3g are1h ar3ehr
    4arei ar3eid ar3ein arein4t 4arem are2n aren3zi are1r2a ar3erh ar3erl ar3ert
    are3sc are1te are3u 2arf ar3fi ar3fl arf1ra ar3fu ar4fus ar3ge ar4gi arg3l
    ar3gla arg4o arg2r ar3gru 2ar3h ar5he 2ari ari4at arie3c ar3im arin3st ar3int
    ar3inw ario4 a1riv ark3amt ark1ar ark3aue ark1l ar3kn 2arko ar4kor ar4kri ark3sa
    ark3she ar3kul ar4let ar4lin ar3mal ar3mang arma3sc arm3aus ar3m2ä arm3erk
    ar3min ar3mit 4armü 2arn ar4nas ar1nei arn2el ar3ob ar3od ar3of ar3op ar3or
    aro3ri aro2s aro2t 2arp ar4pat ar3ph ar3pi 2arr arr3ad arre2g 2ars. 2arsa ars2h
    2ar3si 2arst ar3sta ar3su ar3tag ar3tat 1artd art2e artei5le artel4 artin2 2arto
    ar3tol ar3tra ar4t3ram 2arts arts2e art2sp ar3tue 2aru ar3uf ar3uh ar3ü 2ar3v
    ar4vi ar5vo 2arw ar4win 2ary 2arze ar3zie 1arzn 1arz2t 2arzu arz1w a2s a3sai
    as3au 2ase ase2d a3see as1ef a3seg a3seh a3sek ase2l as1e2m as1ent asen3ta ase2t
    as1eta a3seu 2asg a4sig a4sin 4a4sis 2asit as2k as3kan aski4 as5kir as3kl as3ko
    as3kr as2m as3mal as3me as3mu as3ne as1of as1or a3sos as1ö as1ped as2s ass2a
    as3saf as4sä as3sc ass2e ass3ei assen3t ass2i as3ski ass1p as3sta ass1ti ass1to
    as3str as3stu as4sü 2ast ast2e a4sten as1tep ast3orc a3str as4t3rau ast3räu
    as4t1re a3stü as1tür 1asy as2z aß1er aß1ti 2at 4at. 4ata1 at1ab at4ag a2tak
    at1akt ata2m at1apf a3tass at1au at1ä at2c 4ate. a1tee at1eig atel2 ate3le ate2m
    a2te4n ater3st 4ates 3athl at2hu 4ati4 atie3b atien2 atinen3 at1int at2la 3at2m
    4atma 4atmus ato2m ator3g a1torh at1ort a1tow at1ö at1rä at1re at1rom at1rot
    a3tru at3rü 4at2s at4sche ats1e ats1in ats1p at2t 3attac att1ak att1au at3tell
    att3rau att1rä att3se att4u at3tur a1tub atur1t a2tü a2ty at2z atz1er atz1in
    atz1th atz1w a2u 2au. 2au1a auan1 2aub au2b1al aub3b au1bel au2bri 4auc au3char
    auchs4p auch3ta au3co au1da au1deg au2dr au1du 2aue2 au1eb au1el au2er au1esc
    au2fa auf1an 2aufe. 2aufeh auf1er au1fi auf4ler 1aufn 2auft. au1fu 5aufzeic 2aug
    au1ga au3g2ar 4augeh au1gel au1gem au1ges aug3g au3gla au1go 2auh au1hi auh1r
    2aui au1in 2auj au1ka au1ku 2aul au1la au1lä aul3ese aul3p aul1ti au1lu au1lü
    4aum au3m2ei aum3er1 au1min aum1m aum1o aum1p aums2 aum3st aum1sz au1mu 2aun
    aun2a au3nac au3nä aun2e au1o 2aup4 au1ph 2aur4 au1rau au1rä au1rei au1rin
    au1roc au1rö au2s aus1ah 4ausc au3schl au3schw au3see 2ausen aus3erp aus3k
    au3so. aus1p auss2 3aussag aus3seg aus3st 2auste aus3tie aust2o aus1tr auß2
    2aut. au1ta au3tan 2aute aut3erh au1tie 1auto auto3f au1tr aut3roc 2auts aut1t
    au1tu au3tur 2auu 2au3w 2aux 2auz auz2w 2a1ü av4a avat4 a3vera 2aw a1wä a1wo
    a2wol a1wu a1wü ax2a ax3ab ax3al ax3an ax2e ax2ha axi1d axi1l axi1v 4ay ay2m
    ay1of ay1ste 2az4 a5zei a1zep a2zi azi3g azin3se azi3p azi3v a2zo 2äa ä1am äan2
    ä3ar 2ää ä2b äb1l äb2s äb2t äche1e äch1li äch4s ächs2e äch4t äch2z äck2e äck1n
    äcks2 äde2 ä1des äd1ia ädis2 ä2dr äd2s äd4t ädt4l 2äe ä1em äer3v ä1ex äf2f
    äff1le äf1l äf2s äf2t äft4s ä2g äge1i äge2r äg1l äg2n ägs2 äg3str 1ägy äh1a äh2d
    2ähe ä1hei ähl1a ählap3 ähl2e4 ähl3ebe 2ähm äh4me äh1na ähn2r 2ähr äh1ro ähr3sa
    2ähs2 2äht äh3tr äh1w 2äi ä1im ä1j 2ä2k äko3 äl2 ä1la älbe2 äll1a älm4 älp1 ä1lu
    2äml äm2m 2ämp äm2s ämt2e ä2n4 2än. ä3na än5c 2äne änf3l änft2 2änge änge2r
    2ängl 2äni änk2e änk1l änk2s än5l 2äns äns1c äns2e änse5h 2änz äo1 äo3neu äos2
    äp4 ä1pa ä1po äps1c ä1pu 1äq äqui1 2är. är3a2 är1ä är2b är4bel ärb1le är1c ärde2
    2äre2 är1ei är1el är1ene är2f2 är3fl är2g är3ga är1ind är1int är2k2 är3ka är4ke
    ärke3t ärkom1 är1ma ärme1s är1mis är1of3 är1op är2ri är2s ärsch5li är3se ärt4e
    ärt2s1 är2z är3zi äs2 ä2sa ä2sc äs4e äse1b äse3g äse1h äsen2 äse3ref äser4ei
    äse3reu äse3r2i äse1t ä1si äs3ko ä1skr äs3m ä2s3p äss1c äss2e äss3erk ä2st äs3w
    äß1erk äß2r ät2 ät3a äte1h äte1i äte3l4 äte3rie äti4 ät3j ät3l ät3n ät3ob ät3r
    äts1ei äts1i ät3so äts1p äts3te ät3tis ät3w ä2u2 äube2 äub1l äude1r äu3h 2äul
    äu3li 2äum äum1p äumpf4 äum2s äums1p 2äun2 2äur2 äu4s 2äus. äus2e 1äus2s äuss1c
    1äuß äut2 ä1v 1äx ä1z â2t 4b. 1ba 4ba. bab2w bach4sc bacht4e backs2 bad2m b1adr
    2baf bag4 3bah 4bai bak1er bak1i bak1l bal2 bal1da 4bale 4balis bal3lö bal4s
    2b1a2m ban2 4ban. ban4a b2and band1a band1r 4ban4e bang2 4bani 3ban4k bank1a
    4banl b1anna 4b1an3s ban3tr b3an3z 2bao 2bap 2barb 4bard 2barf bar3ga bar4gen
    2barh 2bari bar3ins bar3k 2barki 2barko 2barkr 2barl 2barn 2barp 2bar3sc
    bar6sche 2barsj 2barsk 4barsta 4barstä bar3tis 2barv 2barw 2barz2 bar3zw b2a4s2
    3bass bast2 3baß 2bat 3bata bat3ent 4bati 4batj bat2o bau1b bau1d 3baue bau3erg
    bau3erl bau1fe b2aufo bau1g bau1h bau1l bau3men bau3n bau1r bau3s bau1se bau1s4k
    bau1z 2ba1w ba2z 1bä 3b2äc bäch1l 3b2är bär3b 3b2äs b1äug 2b1b2 b3ba bbau3er
    bb3b bbbe2 b4be. b3bei b2bel bbe2n bben3s bben4t b3bep b2ber b4bot b3bö 2b1c
    b3che 4b1d2 b2der bde1st 2be. be3al 3beam be3an be3as beb2 2bebeg be1bel be1ber
    2bebet 1bebt be2c 2becc 1bech 3be1d bee2 be1eh be1en be1erl be1ert be1eta 3beete
    be3f 2befäl be1g2 be3ga 2begel 1begl 2bego 1be1h4 be3he 3behö 1bei bei3b 2beid
    bei1f2 bei4fus bei4fuß bei1g 3beih bei3k bei1l2 bei1m b2ein bei3nam be1ind
    be1inh bei1r bei3s2 bei1st beit2s bei1z be2je be1k 4bek. 3bekä be2ki 2bekk 3bekl
    2bekr 2bel. be3las bel3b bel1d be1le 2bele. bel1en be2ler bel3f be1l2i 2belis
    be2liz bel1k bel3lam 4beln be2l1ö 4bels bel1sz 2belt bel1ta bel1tr bel3v 4belw
    be1m 2bem. be3me b1emp 2ben ben3ar ben3da. b2ene 3bened be1nei be1ni ben3k
    ben1ni beno2 be2nor b1entb ben3ti ben3tr b1ents ben3un benz2 3benzo 2bep be1q
    4ber. be1ra2 2berb2 2ber1d2 be1re 2bere. ber4ei. ber3eiw 2berem 2be2ren 2berer
    2beres ber3esc 2berf 4berh ber4har ber4hor be1rie 2berin ber4in. be5ringt be1rip
    2beris ber3iss be1rit 2berm ber3mi 2bern ber3neb ber1ni b1ernt 2bero be3rose
    ber3r2 2bers. ber3st4a 4bert2 ber3tab ber2tä ber3til be1rum 2berus ber3v 4berw
    ber3z b2e1s 2bes. be3sak 1besc be2sen be4sens bes1er be2sh be2sk be3slo bess4
    b3esst. bes1sz be6stein be4s3tol be4strä 2bestu be2sum 2besz beß2 1bet2 be1ta
    be2tam be2tap be4ta3te 2be2th be1tit bet3sc be1tu 1beu4 be1um be1ur 2beü 1b2ew
    3bewa b1ex 3bezi 4bezim 4bé 4b3f4 2b1g2 b3ga bgas3 b3ge1 bgel2 bge3n bge3o bge3s
    b3gu 4b1h2 b2har b3hä bho2 bhol3f 6bi. 2bia bi1ak biar4 1bib2 4biba 2bic 2bid
    2bien. bi1enn bie4s 3bie4t bi1f 4big bi1ga bige2 4bij bi2jo bik2a bi3kar bi3kr
    bi1lat 3bil4d bi1lin 1bill b1illu bil3m 4bi1lo 4bilö 2bin. b1inb bin2e 2b1inf
    bin1gl b1inh bin2n bi3no b1int 1bio1 bi4od bio3g bio2n 3birn bi4sa bi3se bis2h
    bis2m bis2s4c bis1t4 bi3sta bis2tr bi4stü biß2c 1bit. b2ita bi1tic 2biu 4bi1v
    4biw 2biz2 bi1ze 4b1j bjekt3o 2b1k2 bl2 b1lad b1lag b2last b2latt b1law b1län
    4ble. b2lea 4b1leb 2bled b3leg 2bleh 2bleid b1lein blei3sc ble3l 4b2ler ble3sc
    ble3sz b2let 2blich 3blick bli4m blin2 bling4 b2lis 3b2lit bli3to 3b2loc 3blum
    3blü 2b1m bma4 4b1n2 b3na bni4 bnis1c 1bo2 2bo. bo3au bob1l bob1r bod2 bo3di
    2boe bo3ef 2b1of bo3fe bo3gel 3boh 2boj 2bo3k bo3lad bol3an bol3c bol3f 3bolik
    bol3p bol3v 3bom 4bo3ma 4bo3mo bond1e bon1e bon4m 3boo 2b1op4 bo3pr bor2 bor3b
    bor4d bord1r bor3ma b1ort bort5rat 2bos. 2bosk bos1p b2ot boten4e bot2st 3box
    3boys. 1bö 2b1öf 2böl bör4 2b1p2 2b1q br4 2br. 2bra. b1rad b2rah 2brai b2rak
    bra1p bra1st2 brat3er b1ratg brat4sc brau3g brau2t brä2 2bre. bre2c 6b5rechte
    b1re2d 2b1ref 2breg brei2 b1reif b1rek 3brem4 bren2 b3rep b2re2s2 bre2t bre2z
    bri4 b1riem 4brien b1roh1 4broi b1rol bron2 bro4s bro2t brot1t 3b2rö bru4 3b2ruc
    brun2 b1rund brust1 4b1s bsa4 bs1ad b2sal b4samt b3sand bs1än b3sc bsch2 b4schan
    b4schef bs4cu bse2 bs1eb bs1ein bs1ele bs1ent bs1er bs3er3in bser2v b3set bs1ex
    b2sh b3sit b2sk b4ski bs2ku bs3m b2s1of bs1op bs2ort b2sö bs2pl b3spu bs1s2
    bst1ab bst3ac bs2t1ak bst3ank bst3as bs1tät bst1er b3steu bst1h b2stip b3sto
    bs2tob b3stö bs2tr b4strac b2s3trä b4s3treu bst1ro b3stü b4stüb b2s1un b3sy
    bs2zi 4bt b3ta4 btast3r b1tä b1tei b3teil b2tem b2ten b2ter b2tes bt1h b1ti
    b1to2 b1tö b1t2r b1tu b1tü bt2wi b2u2 bubi3 bu3br 3buc 1bud buf4 2bug 2bul2
    4bul. 4bule bull3a bull1t 4buls bum2 b3umk bunde4 2bun2g4 b3ungn bun2s 2buq bur2
    bur4b b4urg burg1l bur4i bur4l bur4n 2bury bus3cha 3busi bus3p 1buss bus4sa
    bus4sä bus1tr bus3u but1al 4bute bu3th but2l 1but2t 2buz bu3zo 1bü bügel3e büh4
    2bül 3bür bür4g 2büt 2b3v 4b1w 2b2y 6by. by1a by2i by1j by1k by1lä by1le by1m
    by1n by1p2 by1ra by1rä by3sa by1si by1so 1byt by1th 4b1z2 b3zei bzeit1 b3zen
    b2zi b3zu b3zü c4a2 2ca. cad2 1ca4f 2cah cal2 calf1 cal3st cal3t 1cam can4 cap2
    car4l car3sh 1cart cas4 ca3se 2cat2 cä2 c2c cd2 c2di c1do 2ce. ce1b ce4d 2cef
    ce1g cei2 ce1in 2cel. cel2l ce3me ce1ne 1cent cen1ta ce1nu cen3un 2ceo ce1q
    2cer2 ces2 ce3sh ce1te ce1u c1f c2h 2ch. cha4 chab4b ch3abi ch1ack ch1ah ch1ak
    chal3ti ch3amer 3chanc ch1ang chang3h 2chant 2chanz 1chao ch1ap 2char. 3charta
    chau1b 2chä ch1äh ch1ärm ch1äs 3châ 2chb 4chc 2chd 2che. ch3eben ch3echt 1chef
    3chef. 3chefs che1g ch1eil ch1eim 2chel che4n chen3ti cher3a 1cherc ch3erfü
    cher3m 2ches ch3ess ch1eta che1te 2cheu ch3ex1 2chf 2chg 3chines ch1inf ch1inh
    ch1ins ch1int ch1inv 2chio 3chip. 3chips 1chiru chi3sta 2chj chl2 chlan2 2chle4
    ch3lein ch1ler 2chm 2chn4 chne4 chner8ei. ch1off chof4s ch1oh chol4 cho3m ch1orc
    ch1ori 2chp chr4 ch1rad ch3raus ch1re3s ch1rh ch1rin 1chron 2chs chs3por chs3tal
    chst3ri 2cht 2chu ch1unf ch1urs 2chü 2chw 2ci2 cine4 cin2n cin2q c1int 3cir cis4
    3cit c1j 4c4k ck1a2 ck2ad ck3an ck1ä ck1ef ck1e4h ck1ei ck2ere ck1erh ck3erke
    ck1err ck1ese ck3f ck1id ck1im ck1in ck1la ck1lä ck1leb ck1lei ck1lin ck1lis
    ck1lo ck1lö ck1luf ck1na ck1nä ck1ni ck1o ck1ö ck1q ck1r ck3rü ck1sen ck1so
    cks3pen cks3tat ck1tal ckt2e ck3ther ck1ti ck1tr ck1tü ck1um ck1up c2l2 classi3
    cli4 clin2 clip3a clo2 3clu c4m2 1co2 4co. 3coa 2cob co3be 2cok co3ku col4 com4
    2com. 2co3mu con1 con2d con4f con2ni co3no con2q con2r con2s con3ta con2ti
    con2tr co3pr cor4 4cos2 2cot co3tä cour2 co4v cover2 cover4b 2cp c2r2 cre2 1crew
    cro1c crom4 2cs4 c1se cst2 c1str 2c2t cti2 ction3 c2u2 4cud 2cul cum2b 3cup3
    cur4 2cus 1cy2 c1ze 2d. 2da. da1a da3b d2abä d2ab2l da4bre dab2rü d1ac da1ca
    dach3a 4dachse dad2 da3dr da3du d1af da1fe da4fo d1ag2 da3ge da1h da2ha da3in
    da3kr 2dal dal2a d1alar dal2b2 dal3bl dal2k dal2m d1alp d1alt 2dam. 2dama 1dame.
    da1mi 1damm d1amma d1ammä 3d2amp dampf8erf 2dams d1amt da3nac d1an1d2 da3ne
    da5neb d3anei 1dan2k dan3kla dank1o dank1r dank2w 3dann. d1ans 2dant dan1ta
    2danz 2d1ap da1pa da3pe d2aph dap1p 2daq dar3a darb2 d3arc dar4d darf4 dar3g4
    dark2 d2arl darm1a darm1i dar3n d2ar3s dar3tu 1dar3u 4darz da3s 2das. 2dasc
    das2h das3se da4t2 d3atl 4datm 1dau4e d1auf d3aug dau2p 2d1aus da1we d1ax da3z
    1dä2 3däc 2d1äh 2däl 2d1ämt 2dän d1änd d1äng 2d1äp 2däq 2där där3b där1t d1ärz
    4d1äu 2däx d1b2 d3ben d2bol d1ca d1ch d1cu 2d1d2 d3dä d2de2 d2dy 2de. de2ad
    de3an de3at d2e1b2 2debe d3eben 1debü 3debüt 1dec de2ca 2de1ch 3deck de1d dee2
    de1eb de1eg deen3g de1er de1es de2fau 1defek d1eff 3de2fi 2de3fl 3defla 2defr
    de1g2 de4guss de3he 1dehn d1ehr dei2 1d2eic d2e1im de1ind de3inse de1k de2kal
    de2kan de2kl 1dekod 1de4kor de2kre 2dela de2l1ag de3land del3aug del1än del1d
    de1leb del1ec de3lege de3lein de1les del2fi d1elfm del4gar de1lie 3delik del1k
    del4lan del2le dell3eb del3leu del2lö del1ob de2lö del4ph del2s1e del2so del2s1p
    del5ster delt2 del1tr de3luk 2dema de2met de1min 1demo de3mon de1mot 2d1emp
    de2mut de2mü 2den. de1n2am de1nas dend2 2dene den3end den3ga d3enge. de3nit
    3denke den3kel denk3li 1denkm de2not den4s3en den2sk den3ska den6s5tau den3tat
    den1th den3tie den3tr 2deo de2o3b deo1l 2depf 1depo de4pri de2pu de1q der3ap
    de1ras de1rat der3bee der2bl d1erbs der3c der3eis de1reo der3ero d4erfi d2erh
    de3ring der3k d3erklä d2erm der4mau der1ni de1ros ders2k 4dert der3tan dert5end
    der3the der3ti de2ru der3v der3z de2s1a de3sac de4sam des3an 3desast des1än
    des3b de3sc de3see des1en de2sh de2si de3sid des1in des3n de2s1o de3so. de2s1p
    de3spe dess2 des3sa des3sel des3sor dest5alt de5stang de3star de3stat de5stel
    dest5rat de3stri de2s3u det2 de1tal de1tan 1detek de1th de1ti de2tro de1tu d1etw
    1deu de1un de1url 3deut de2vi 1dex d1exi d1ext de2zi 2dezu de4zu. 2dezü d3f2
    2d1g dga4 dge1 dge3m dge2t dget1e 2d1h2 d2hab d3he dher2b d2his di2 2di. dia1b
    3diag dia3sk 1dic di3chl did2 d1ide 1dieb di3elek die3li di4en dien1d 4diens.
    dienst5r die4r dies1c die4t dige4 di3gene 1digi 3dig2m 1dign di4k2 di5kar dil2
    dils3 1dim 2d1imb di3met di3mo 2d1imp 2din2a d1ind 2dine d1inf d1inh 2dini
    d1init 2dinn 2d1ins d1int 2dio. dio1b di4od dio1l dio3nat dior2 2dios dio3sen
    3dip di3ph di3po 1dir 2dira 3dire 3diri d1irl dis1a 2disch 3disk dis2la 1dis1p
    di3sper d3isr dis3sen dis3sim dis1so dist4 di3sta di4ste dis1to dis1tr di4stra
    di3su 1disz dit3erl dit3erm dit3ers dit3r 1div di4vi 3divis 2diw diz2 d1j d1k
    2d1l2 d2lat d2le d3lea d3leb d3leg d3leh d3lei d3lek d3lert d3leu d2lig d3liga
    dl3m d3lo2 dl3s d2lu d3lud d3luf d1m dmen2 dmin2 2dn2 d1na d3nä d1ne d3ni4
    dnis1t d3no d3nö d3nus d3nü 2do. 2d1ob d2oba d2obr 1doc 2doch do1chi 3docu do3d
    do2de 2doe d1of do1ga do1ge 3dogm 1dok 4doka 4dokr do1lei 3doll2 1dolm 3domiz
    do2n2 donau1 3donn do2o dop2 do1pa d1opf do1ph 3dopi 3dopp dor4 2dor. d2ora
    dora5b do1rat 2dorc d1ord 3dorf dorf1a 4dorff. 2d1org dos4 do3sa 3dose dot2h
    dot1o do1tr do3un 2dov dow2 do1wi dows1 3dox do2z d1öd d1öf d3öl3 3dör4 dös1c
    d1p4 d1q 1dr4 dra2 2dra. 2d1rad drag4 d1rahm 3d2ram d1rand 2dra3r 2dras 2drat
    drat3z 4drauc d2rauf 4draup 2drä. d1räd 2dräe d2räh d1rät d2rea d3real 3d2reck
    2d1ref 2dre2g 3dre4h dreh1r 4d1reic drei1g drei1l drei6lic drei3t d1ren d3rep
    dres4 2d3rez 2dré 4d1rh 2dria 2dric d1rieg drieg3l 2drik 4d1rind d1ris 3drit
    4d1ritu 4dritz 2dro. 2drob d1roc d1rod 3drohe 2droi dro1k 2drol 2dron dro1p
    d1rose d1rost 4d1ro2t d1rou d1ro2v drös1 d1rub d2ruc d1rud 4druh d2ruh. 4drui
    4drund 4d1rut d1s ds1alk d4s1amt d2s3an ds3assi d2sau ds1än dsch2 d4schin d3s2co
    dse2 ds1eb dsee1 ds1ef ds1ehr d3sei ds2eig ds1ein d4seins ds1ene d2s1eng d2s1ent
    d2sera ds1erf ds1erk ds1err ds1erz ds1eta d2sex ds2ha ds3han d2she d2shi d2sid
    d2s1im d2sind ds2inf d2sk d3s2kan ds3ke d3skul ds3m d3so. d2s1op ds1ori d2sö
    d2s1par ds1pas d2s1pä ds3pres d3spri d2spro ds1s dst4 d4s3tabe d4stag ds3tauf
    d4s3täti d2ste d3stei d3stel d4stem d4sten d3stern ds1th ds2til ds1tis ds1ums
    d2sun d2sz d1ta d3t2ac d3t2as dt2ax d1tä d3tea d1tec d1tee d1tei d1tel dte3na
    dte3rei d1term d1teu dt3ha dt3l dt3n d1to d3tou d1tö dt1r dts2 dt3sa dt3sc dt3st
    dt1t d1tu d3t2ur d1tü dt3w d3ty dt1z du2 2du. 1duf2 2d1ufe d1uh 2dui 1duk 2duku
    1dul d1umb 2d1umd d3um1e 2dumf 2dumk 2duml d2ump 2dumr d1ums d1umv dun2 2d1und
    d1unf dung4 d3ungl d1uni 1dunk dunst3r 3duo du3pr 1dur2 dur4b 3durc durch1
    2d1url dus2 duse2 dus3k duss2 2dust du3sta duß2 1duz 1dü 2düb dür4 d1v2 d3vo d1w
    d3waf d3wal d3wan dwest1 d2wic d2wil d2y 2dy. dy1c dy3d 3dyn dy1ne dy2s1 2d3z2
    dzu2g 4e. 2e1a e3ab e4abi ea3gat ea3kr eakt2 eak1ta e2al eal1li ea6logis eal1ta
    eal1ti eam3a eam3b e2ame eam3o eam3to e3anb e3ang ean2n e3ant ea3nu e3anz ea2p
    e3arb eare4 ear3ene ear3ma ear3ra e2as eas3z e3ath eats2 e4au. eau2b e3aue e3auf
    e2aus. eau3sc eau3so eaus1s e2av e3ä2 2eba e2b2ak eb3ber e1be3a ebe1b 2ebec
    2ebed e1bee e3be2i e5beku 2ebel ebe2n ebens3e e2bers e1beru ebe4s3an e3besc
    2e3bet e3bew e1bi e2bit 2e1bl eb2laß e3ble eb3ler eb2leu e3blie eb1lo eb1ob
    e2bos ebot2 e3bö 2e1br eb1rei eb2ri e3bro e3brü eb2s eb4sche ebs1i ebs1o ebs1p
    ebs3pa eb3sta eb4stät ebs5temp ebs1th ebs1ti ebs3tot eb3str ebs1u 2e1bu e3bur
    e3bus eb2z ec2 2eca 2e1ce ech1ä 2eche e3chef ech1ei ech3erz e3chip ech3m e2cho
    ech1ob echo1l echt4ei ech1uh ech1w eckan3 eck4har eck1n eck4sen eck4sta 2eckt
    2e3cl 2eco e2con e1cr 2ect e1d2a4 e3dau e3dä ed2d ed2e e1deb e1def e2de2l eden2e
    eden4s3e eden4s3p e2de2r ede3rec edert2 e3desi edi4 e1dif e2dik 2edip e3dir
    e3div e2diz ed2n ed2o e1dö ed2r eds2ä ed2seh ed2s1es ed2s1o ed2s1p ed2s3tr ed2su
    e1du e3due 4ee ee3a ee1b2 ee1c e1eck ee1di ee1do eeds2 ee1e e1eff ee1g ee1he
    ee1hu e1ei2 ee1k eel2e e1elek ee1leo ee3me eemen2 ee1mi e3emp ee1nad ee1nä e1end
    een1da ee3ner een2g ee2ni een1sh e1ent een4wi e1enz e2ep ee2pe e3epi ee1q eer3as
    ee1rat ee1rau e1erbt e1erd e2ere eer1ei eer3eng eer2es3 ee2ri ee1ro ee1rö eer2ös
    eert2 eer3ti eer2u e1erz ees2 ee1sh ee3sp ee3st ee1ta ee1u ee3un e3ex ee3z e1f
    2ef. 2efa e3fab ef1ad e3fah e2fak e3fal ef1ana ef1ank ef1arc e3fas ef1aus e3fäl
    ef1äu 2efe ef1eb efecht4 e3feie e2fek efe4l efel3ei ef1em ef1ene ef2er efe3sc
    2eff. 3effek 1effi 2efi e3fib ef1id ef1ins ef2ke 2efl e3f2lu ef3n 2e5f2o efon1t
    e3fö 2efr e3f2ra ef1rea ef2ri ef1rom ef1rot efs2 ef1so ef3sp eft3an ef1tel 2efu
    ef1um 2efü ef3z e1ga ega3d eg1am eg1aus eg1d eg4e e3gei e1gelä egen1a e2genf
    e2geni ege2ra ege3rad e3gesc e3gese eges3to ege2t ege3ter ege1u e3gew eg2fri
    e1gis e4giss e1gl eg1lo eg2mu eg1nä e1go e1gö e3gro eg3se eg4sei egser1 eg3si
    eg4sin eg4sto egung4 e3gü 2e3ha eh1ach eh2al e2h1ap e4hav 2e5hä eh1eff ehe3k
    e3helf e3henk ehen4t3 1ehep eher4an eher2b e3herr2 e3herzt e1hi e3hie eh1int
    e5hir eh1lam eh1lä ehl3ein ehl2er eh1lo ehl2se 2ehm eh1ma eh1na eh1ni eh1no e1ho
    e3hol 2e3hö ehr1a ehr1ä ehr1ec ehr4erf ehr1ob eh1roc ehr1of ehr3sch ehr3z ehs2
    eh1se eh1sh eh3sp 2eht eh1ti eh1tu eh2tun e1huh e1hul eh1um e1hup e1hus e1hut
    e1hü eh3üb eh1w e3hy eh3z 2ei3a4 eian1 ei2bau ei3berg ei1bie ei1br eib3rat
    ei1bus eib3ute ei1ce eichs7test e2id eid1a eid4ein eid3err 2eidn eid3st ei1due
    eie4 ei1eb ei1ec ei1el 4eien eien3st ei1erz ei3esk ei3fest ei1fla 1eifr 2ei1ga
    eig2e 4eigeno 2eiges 2eigew ei1gie 1eign 2eigru eigs2 2eigt 2eigu ei3ha ei3ka
    eik2ar eik2l ei3kla ei3ko ei3kr ei3ku e2il 2eil. ei1la ei1lä 2eilb 2eil1c eil3d
    ei1leg eil3f2 ei1lie ei1lis ei1lit eil1l eil3m 2eiln ei1lo ei3lö eil1se eil3z
    eima4 eim1ag ei1mal eim3all eim5alp ei1mi eim3m ei1mo e1imp ei1mut e4in. ein1al
    ein3an ein1ä ein1da ein3dr e2in1g2 e1init e2in1k 3einkä ein3n2 ein1ob 3einri
    e4insa 3einsat 5einschä ein2s3z ein3teg 1einu ei1o eip2 ei3pf ei1q 2eir ei1re
    e1irr eis3erw eis3he ei1sir ei3sit eis3ke eis3me eisof3 ei3sp ei5spru eiss6lun
    ei3sto e2iß eiß4lun e2it 2eitä ei2ter eit1h eit3t eit3um ei1tü eit1z 2eiu ei1v2
    eiz1in eiz3z 2e1j e3ju ek4 e1ka 1ekd 6ekdote. 4ekdoti 2ekdö e1ke e3kee e3ket
    e3key e1ki e3kif e1kl e3klo e3kn e3ko e3kra e3krei 2ekt ekt4ant ekt3erf ekt3erz
    ekt2o e1kuc e3kum 2el. e1la el3aben el3abi el3abu el3ader e3ladu el1af el1aho
    el1ak el1am el1ana el3anda el1ans e2l3anz 2elao e2l1ap el1ar el3ari e2las.
    ela3su e3lats elau4 e2law e4lay e1lä elber4g el2bis el1c eld3erh eld5erst el1di
    el3die elds2 el1dy 2ele. e1legu e3leh 2elei e2lei. e3leid el5eier. el5eiern
    el1ein e3leine el4eint e3leis e3leit e2lek ele3ki el3el ele3la 1elem ele3ma
    el1emp 2elen. el1ent elen3z ele3po eler2a e2l1erd el1erf el1erg e2l1erk el1erl
    el3ernä el1err e2lers el1ess e2let el1eta el1eur 2e1lev e2l1ex 1elf. elf2l 1elft
    elg2 el3h el4har el4hup e3lic el1id e4lien e1lif e2lig elig2m e1lil el1ita e1liz
    2elk el3ki elks2 elk3sc el1ku el3kü 2ell el1lac el1lad el1lag ellaufeigen8
    el1läd el1län el1lei ell3ein ell2er el1lo el4log ell3sp ellzu4g elm3ein el3mo
    2eln el1ni 2e1lo elo1m el1ope e2l1or2 elo2s e1lö el3p 2els el1se el1tat elte4
    el1tee elt3ent elter4b el1the el1ti elt1r el1tri el1tro elt3sa elt3se elts4k
    elt3s2p el1tur 2elu e1lud e3luf e3lui e1lul el1ur e1lus el3use e1lü el3va ely2
    el1ya 4em. 2e1ma e3mac e3mal. em1alk em1anf em1ano em1ans 3emanz e3mar4 e3maß
    e3mä em2b em3be em3bi em3bu em3bü em2d emd3a e3mec e3med e2men emen2t ement3h
    emer2g em1erw 1emeti em1ex emge2g e1mil emin2 em1int em2ki em3lu 2em2m emme2
    emmen3t em1n em2ni emo1k e1mol e1mon e3mö e4möl em2p em3pa em8pathie 2empe
    1emp1f4 em3pfl emp3ler em2s em3sc em3sel em3so em3sp emt2 em1th e1mu 1emul 2emü
    e1my 6en. 2ena4 en1aa e1nac en1ack en1agi e1nai en3ak en1al4 en1am en3ane en3anz
    e2n1ap1 e2nar en3are en1ass en3ast enat4 en3atl en3att e2naut en1ax en3az en1ä
    e3näc 4enb en1c en3chi 1ency 2enda en3d2ac en1dat en3desh end3ess en3desw en1die
    en1din en1do end4ort end3ras end3s4l end3s2p end3sz end1t en1due en3dus en3d2ü
    4ene. e1neb en1ec en1ehr 4enei ene3k en1el en1ent en4entr en1ep en1erb en1erd
    en3erei 1energ en1erk en1erl en3ermo en1ers en1ert en1eru en1erw en3esc en3ess
    en3est en1euk en1eup 2enf enf2a en3fe enf2u en1g 1engad 3engag eng2i 1engp 4enh
    en3hi 2eni2 e1nic en1id e1niet en3im en1in e1nip e2nisc enit2 e3niv 4enj en3je
    en2kli enk2ü 4enm 3ennal en1nei e1no en1oa en1ob e2n1oc en1oh en1ok eno2m e2nom.
    e2nome e2noms en1op e3nopp en1or enost1 e1nö en1öd 2enp 2enr 4ens. en3sac en3saf
    en2sau en3sek en3sel 3ense2m ens3ere 2ensi en1ske 2en2so en3so. en3son en3spe
    en3s2po enst5alt en4s3tät en3stel ens5test en3stoc ens5trie 2ent. 4enta ent4ag
    enta3go en1tai ent4ark en1tas 1entd en1tee en3terr 1entf 1entga 3entgeg ent1hi
    en1thr 2enti 3entla 1entna en1to ent3os en2tr ent3rol 3entspr 1ent3w 4entwet
    1entz en1u4 e2nua e3numm e2nun e3nut e1nüg enü3st 2env 2enw enz1ec en1zeh
    enz3erg enz3erk enz5erst enz3l e1o2b1 eo3bi eo3bl eo3bo eo3bu eo1c eo1da e1of1
    e3of3f eo1g e1oh eo1k e2ol eo1m2 eon2h e3ope e3opf e1or2 e3ord eor4g e2orge
    e3ort eo1s eo3sc eo8sophie eo1t eo8typie. e1ov eo1w e3ö e1p2 e3pas e2pec e2pes
    epe3ta e3pf4 e3ph epi1 e2pia 1epid e3pir 1episo 2e3pl 1epoc e3poi e3pol e2pot
    2ep4p epp1l 2e3pr e4ps e4pt ept4u 2epu e3pun 6er. er1a e2rab era3ba era3ber
    e2rach e2rad er3adm er3adr eraf2 e2rai er3aic e2r3ak e3rake er3alke er3all era4n
    er3anf er3apf er3apr erar4 er3asi era4sp e1r4ast e2rath e2rati era1tr erau4
    e1raub er3aue er3aug e3raum e1rä2 e3räd er1äh er1äm e2r1äs er1ätz erb2e 2erbed
    er3benz 2erber 2erbew 2erbil er3blu 2erbo 2erbü 2er1c erch2o 4erd. 1erdb 2erdec
    erde3in erd3eng erd3erw er1di4 erd5lo erd3st 2erdy 4ere. er1eb e1rec er3echs
    er1eck e1red er1edi e1ref er1eff e3reg er1eh 4erei. erei1b e2r1eig e1reih
    e4reins er3eis. e2r1el ere2le 2ere2m 2eren e1renk e1renn eren2z erenz7end er1ep
    2erer er3erf er1err er1ers er1erw 2e2res er1ess er1eß er1eti er1eul e3re2v
    er3evid ere2z 4erfam 2erfe 4erfl erf4r 4erfür 3ergebn 4ergehä ergel4 erg3els
    er3gem 2ergen erge3ru 6erges 2ergew erg3ise 4ergl erg3ler erg3rad 4ergrem 4ergru
    erg3s er3gu e2r3h 1erhab 4erhals 2erhar 2erhau 2erhin 4erhöhe 2erhu 4erhü 2eri
    4eric 4erie erien3 4erin. er1inb er1ind er1ini er1in3k er1ins er1int e2rite
    e1ritt 2erj er3kar erken2 2er3ki 4erkla 2erkli 2erkre erkt4 erk1ti erk3tr 4erl.
    6erlad 2erlag 3erlebn 4erlei. 4erleit er4ler. 2erli 4erln 2erlo er1m2 2erma
    ermen2 erm3ers 2ermo erm3t er3mü 4ern. 4ernd 3erneue erno2 ern1os e2r1ob er1of
    erof3f ero1g ero3ge er1oh ero2i er1oly e2roo er1op 2eror e1rou er1ox e1röh 2erök
    er1ös 4erpa er3ph 4erreis 2erren 2erro 2errü 4ers. ers2a ersch2 4ersh ers2i
    er3smo er3sn ers2p 4erstu 4ert. 4erta ert2ak er1tal er1tar er1tat ert3erf er4tes
    er3them er1thr er2tie er3tik ert3ins er1tis er1to ert3s2e ert5s2p 2ertu er1tue
    er3tun. e1ruc e1rud eruf4s er1uhr er1um er3uml e1rumm er1und erung4 er3unt
    er3up. er3ur er3use e1rut er3uz e1rü 4erv 3erweck 2erweg 2erwes er3zel er3zem
    2er3zu 2erzü 4erzy 6es. es2ac es1ad e3sai e3sal e4salt es2an es3ant esa4s e3sat
    es3ato es1av es1ax es2bie es2ce esch2 esch4n e3scho e3schr e4sco e4scu e1se
    es1ebe es1ehr e2s1ein es1eis e3sek e3selbe e2se2m ese3n4ac esen3ke esen1o e2sent
    e3seq ese1ra ese3rei e6sertio eser1u eser2v es1eta es1eva 2esh e2she e1si e3sic
    es1il es1ini es3int e3sit es2kat e4sky es3m e1so eson4 e2sop e3sorp es2ort es2ö
    2esp e3s2por es2pot es4pra e3spu 4ess. es2sa es4saue 1essay 2essä 2es3sc 4essem
    8essenziell. ess4ere ess3erg 2essk ess4lin 2esso es1soh es1soz 2essp ess1pa
    ess3tie es4su essub2 essub4j es2sü 4est. 4esta est1ak e4starb es3taum es2te
    e1stec es3teil est5eink e3stel e4sten est3eng est3erh est3ess e1stil e2s3tip
    est3ori e1stro es3trop e1strö es3trun e1stu e3s2tü e1su es1um es1ur e3sy es2zi
    e2ß eß2bl eßer2e eß4ki eß6kopf. eß4men eß2pu eß3ri eß1ti 2et. e2tab2 e3tac e1taf
    et1ami et1ano et1ant e1tä et1äh et2chi 2ete e3tea e5tec e1tei e3teil et1ein
    e3tek e1temp e2ten etend2 e1term e3teu et4fri 2eth. e5thea 1ethn eti4 etin2
    e1tip et4lin et2n et3ne et3ni et1of3 e2ton. e1tone e2tons e1tonu eto1ph e3tö
    2e1tr e4traum et1rec etrei4 et1rel et3res et2s etsch3w et3se et3si et3so et3sp
    et3str et3su 2et2t et3tag et4th etto3r ett1ra ett1ro ett1um et1ups e1tur e1tus
    e1tü et3wo 2et2z etze4 eu1a euan1 eu1b2 eu3ba eu3be euber4 2euc euch2 2eud eu2dä
    eu4den eude1s eu1eng eu1ent eu3ernt 2euf4 eu2fer eu1fu euge2n eug2er eug3g eu1gi
    eu1gl eu1go eugs2 eu1h eu1in eu1ku eul2 eu1la eu1li 2eum2 eum3b eu1mo eums1p
    eum3st eu3mu eun2e e3ung eunk2 eu1o eu1pe euper2 eu3pf 2eur. eura4 eur1d 2e2ur2e
    eu1rei 2eurs eur1t eu1ru eus2 eu1sel eu1si eu1so eu3sp eu1str 2eut euter2 eut2o
    eu3tor 2euv eu1zel eu2zo eu2z1w e3ü e1v2 evan2 2eve e4ven e2via e2vie 2evo e2von
    e2vot e1w e3wa e3wä 2e3we ewei4 ewer4 e3wir e3wo e3wü 4ex. 1exam ex3at 2exd
    ex1er 2exie ex1in ex1l 3exp 2ext. 2exta 2extv 2ey ey1c ey3d eye1l ey1k ey1l ey1m
    ey1na ey1p ey3sa ey3sc ey1t 4e1z e3za ezei2 ezen2 e4zent. e4zente e3zep e2zer
    e2zes e3zess. e5zessiv ez2m e3zon ezo1s e3zu e4zue ezu2g e4zu2m e3zü e3zw e3zy
    ez2z é2c é1h é1lu é2r é1t é2te é1u é1v é2z 4f. 1fa2 2fa. 4f1abl 3fac fach1i 2fae
    fa3ec 2faf fa3fa f2ah fahr5tes fak2 fa3ki f3aktio fal4 2falg fall5erk fall5tes
    2fam. 2fame famen2 2famie f1amt fan2f fan2g fan1si 6fantil. 6fantile f3anz
    fan1zi 2f1ap far4 farb3l farb3r farr3s f1art f1arz f2as fas4a fas2e 3faß 2fat2
    f3att f1auf f3aug f1ausb f2av 3fax 1fä fähr3u f1älte 2fäq f1ärm f1ärz 2fät f1äug
    2f1b2 fbe1 2f1c 2f1d2 fde2 4fe. fe1b f2ech f1eck 3fe2d fe1em fee1t fef2 3feh2
    2fei. 1feie 2feien 4feis fe2l fel1a 1fel2d 3feldz fel5eise f3elek fel1er fel3f
    fe3lie 3fell. fel1la fel3läu 3felle fell3er 3fells fel1o fel4soh felt2 f5eltern
    fel1tr fel1tu fem2 f1emp 2fe2n fen3a fen6bart fen3s2a fen3s2c 1fensi 4fensic
    fens2t2 f1ent fen1ta fen1te fen1z fer2an fe2r1ä fer2de f2ere 4feree fer3eis
    fe3rel f1erfa fer1id 4ferin fe3ring 2ferno fe1rol fer3re fers2t f2ert fer3tie
    1ferts 1fertt fe2s 4fes. fe3sa fes2t fest1a fest3ei fest1r fet4 f1e2ta fe1tie
    fett3a fett1r feu2 3feue 2feui f1ex 2fé 2ff f3fal ff1ans ff1arm ff2art ff3at
    ff1au f3fäl f2fär ff2e4 ff3eig ff3eins f3feld ff3emi f3fensi ffer4sc ff3f2
    fffel2 f4fic f1fis ff1lag ff1loc f1flu f1flü ff1ox f1fr ff2rä ff4ri ff1rö
    ff4schr ff1so ff2st ffs3tel ff3str ff1ti ffus5s 2f1g2 f3ga fge1 fge3b 2f3h2 2fi.
    fia3b fich2 fid2 2fidi 1fiel fien3 2fier fi1erf 2fif fi1go 2fih 2fij 2fik fi3kl
    fik1o fik1te 3f2il 4filag fil3an 6filaufb fil3da 4filig fil1l fi1ma 1fin2 2fin.
    3fin4a 4fine fing4 fings2 4finitu fin3sc f1int 4fio4 3fir 4fi1ra fis4 fisch3a
    fisch3o fisch3w fi3sp 2fitea 4fitenn fit1o 2fitou 2fitr 2fi1tu 2fi1v 2f1j 2f1k2
    fka3k fl4 fla4 f1lad flag2 flan2 1flä 3f2läc f3län 2fläu fle4 f3leb 2f1lein
    flekt2 2flé f2lig 1f2lim f1lind f1ling flo2 1flop f2lor 1f2lot 2f3löf 3fluc
    flug1a f2lü f3lüd f3lüm 4f3m2 fmen2 2f1n2 f3na f2ne f2nu 3fo 4fo. 4foa 4fob
    fo1be fo1br fo1bu 4foce 4fo1d 4f1of1 fo1ha 4foi fok2 fol4 4fo1la 4fom fo1ma f2on
    fon1di fon2e fon1te 4f1op for4 fo1rad 4forc 6f3org form3ag fort1 4fos 4fo1ta
    4fote fo1tel 4foth 4fov 4fow 4foz 4f1öf 4f1ök 2f1öl för4 2f3p2 fpen2 2f1q fr2
    frach2 f1rad f1rah fran2 f1rand 2frar fra3sc 2frat 1frau. frau3c frä2 f1räd
    3f2rän 2fre. 2fred 2f1ref 2freg f1reic frei1g freik2 frei4tä f1rep fres4 fre3sc
    3f2reu fri4 1fris 2froa fro1b f1roc 1f2ron fron1d fro4s 3f2rös fruch4 f1rum
    früh3m 4fs f2san fs3ar f2s1as f2sauf f2saus f2saut fs2äc fs3b f3sc f4schan
    f4schef f1se2 fs1eb fs1ehr fs1ein f2s1em f2s1ent f2s1er f3serv fs1eta f1si fs3ke
    f1skie fs3m fs1o f3span f2s1pas fs1pel fs1pen f2s1ph f3s2pl f3spor fs1pr f2spre
    fs2pri f2spro fs2pru fs1s2 f1st fs1tät f4stäti f3stei f3stel f3stern fs1th
    f2stip fs1tis fst2r fs3tres f3s2tro fs1trü fs1tut f4stüte f1su f2s3un f3sy 2ft
    f1ta ft1af ft1al ft1an ft1ar f3tat f3tä f4t1äu f1tec ft1ed fte3g ft1eh f1tei
    ft1eig ft1ein ft3eis ft1ent ft1erl f1term ft1eti ft1h fti2 ft1id f1tis f1to
    f3ton ft1op ft1or ft1ot f1tö f1tr ft3ric ft1ro ft1rö f3t2ru ft2s1 ft4sam ft5same
    ft3s2c ft4sche ft3st ft4stä ft4stei fts2ti ft4stri ft3sü ft1t ft1url f1tü ft3wo
    ft3z2 fu2 2fu. 3fuc fuch2 3fug 1f2uh2 ful2 2fum fun2 f1unf 2fung4 f3ungl f1uni
    fun4k funk1r funk3tu f1unm f2ur 4fur. 4furs fus2 1fus4s fuss1p 3fuß fuß1er 3fut2
    2fuz 3fü 4füb füd2 füh2 fül2 fün4 für3b 2f1v 2f1w fy2 2f1z f3zei f2zi f3zu f3zw
    f3zy 4g. 4ga. ga4b2 2gabf ga5bi ga5by 2gabz ga1ch 2gad2 ga3deb 4gae ga1fl ga1fu
    2gag 2ga1h 2gai 2ga3k 2gal. gal4a gala3d g1alau gal2g gal1la gal3m g1alu ga1ma
    ga2me gamen4 gamm2 2g4amo 2g1amt 4gan. 2gana 2gand 2gane 2g1anf gan2g 4gangeb
    gang4s3p gang4st gang3u 2g1anh 2gani 2g1ank g3anku 2ganl g3anla gan2n gan2sc
    4ganw 2ga1p 2gar. 2g1arb g1ar4c gar4d 2gari 2g1arm gar4o g2ars g1arti ga1ru
    gas3ab gas1al gas1ei gas3m gas4mu gast3el gas4trä gas1tu gas3z gat2a ga1tab
    2gatm 2gato ga1t2r 4g1auf g3aug g2auk g1aut ga1v 2ga1w 1gä4 2gäne 2g1äp g1ärz
    2g1b2 g3bl g3bu g3bü 2g1c 2gd gd2ad g1di gd3l g1do g2don g1dö gds2 gd3sz gd1t
    g1du g3dy 4ge. 1ge3a 2gean 1geä 1geb ge3ba 3gebä gebe4am ge1bel ge1bes 3gebi
    ge5bigk gebot4 geb2r 3gebü 5gebüh ge1c 1ge1d 4ged. 1ge1e2 ge3ec ge2es 1gef2
    ge3fa 2gefar 3gefec 2g1eff 3gefü 1geg4 ge1gel ge1gem ge1ger ge1ges ge1gu 3geh
    ge1he ge2her geh1m 2geif ge1im ge1ins ge1inv ge1ir geis2 1gej 2gej. 1g4e3k g2el
    2gel. 2gela. 4gelas. 4gelba gel5bert gelb3ra 1geld. ge1le 2gele. 4gelei. g3elek
    gel3ers 4geles. 4geless 4gel3f 2gelg 2gelh ge1li 2gelik 2gelin 4gelk 4gell2
    gel1li gel3m 4geln 2gelo. 2ge3lor 2gelp 2gelr 4gels gel3sa gels2p gels2t gel3ste
    gel1sz gel1t2a ge1lum 2gelus 4gel3z2 g4em4 2gem. ge1me 3gemei gemen2 ge1mi ge1mo
    ge2mon ge3mos ge3mu 5gemü 2gen. ge1na gen1ac 2genap ge2nat gen4aug ge1nä 2genb
    2gend gen2d1r gen1eb 4ge1nec gen3ern gen6erwe 2geng 2genh 1geni 3genie ge1nis
    gen3k 2genm gen1n 1genom ge2nov 2genp 2genr 2gens gen4sam gen7stern gen1sz 2gent
    gen1t2h gen3tr ge1nu 2genv 2genw 2genz gen3zin genz1t gen3zu 1geo 2geoo geo1pa
    ge1ou 1geö 1ge3p 3gepu 1ge1q 4ger. ge1ra 3gerä ger1d ge3rec ge1rei g3ereig
    ge3remp ger3ent ge1ret 2gerg 4gerin. ge3ring ge1rit 2gerl ger1no ge1ro 1gerol
    ge1r2ö ger3re 2gers g1ersa ger4sto ger2u 1geruc ger3v g1erwa g3erwer ger3z g2es
    2ges. 2gesb 1ge3s2c 1gese ges3elt ges1er 2gesf ges2i 2gesk ge3s2p 2gesr ges3ser
    ges3ste ge1s2t 2gest. 6gestan. 6gestani 6gestans ge3ste ge4ster ges3th ges3z
    1ge1t 2get. ge3tag ge3tan 2geti. ge3to ge3tr getrie4 get3sa get4u get3z 2ge1ul
    1geü ge3v 1gew g5ex 1ge3z 2gé 2g1f2 g3fä g3fes g3fet 4g1g2 g3ga g3gä g3geb g3gef
    g3gela g3gep g3gew g3glo g2go g2gri g2gy 2g1h g2hai g2har gh2e gheit4 g2het
    g2hie gh3l gh1n gh1te gh1w gi2 gia2 3gib2 4gibi gie1b gie3be gie3d gie3f gie3g
    gie3h gie3i gie3l gie3m gien2e3 gie3rec gie3res gie3st 3gieß gie3t 3gif 2gig
    2gik gi3ko gi3k2r 3gil2 4gin. 2gina g1ind 4gine g1inf gin2g g1ins g1int 4gio
    3gip 3gir gis2 2giss gi3sta 3giß git2 2g1j 2g1k2 g3ka g3ki g2kok g3kr 4gl. gla4
    2g1lab glad2 g1lag 2gland glas3k glas1p glas3ti 3glat glau2 1g2laub g1lauf 1glä
    g1läß 2g1läuf 1gl4e 2gle. 2gle3a 2g1leb g3lec g1leg 2gleh glei4 2g1lein gleit5r
    2g1len 2gler 2gles g1lese 2gleu g1lev g1li 3g2lid g2lie 2glif 2glig 2g2lik 2glil
    g2lim 2glin 2g2lis g3list 1g2lit 2g2liz 2g2loa g2lob g3loch 2g2lo1k 1glom 2glo1p
    g2lor g2lot 2glöw 2g1luf g1lus 3glü g1lüg 3g2ly4 2g1m2 g3mah gman2 gma1ri g2mat
    g2me g3mel gmen2s g3mes gn2 3gnad 1gnä 2gne4 g1nel g2nen gn3g 2g1ni4 g2nie g3nis
    gno2 g1not g1num gn3z 4go. 3go2a 2go2b gobe2 2goe g1of1 gof3f 2gog 2g1oh 2go2i
    2gola 2gole gol4f 2golo 2gol3s 4gom go1ma gon2a go1nau gon2d 2goni 2go1p g1opf
    go2pos gor2 4gor. gor4b 2gord 1gori go2s2 gos3i go3st 4got2h go4ti go1tr
    gott5erg 3gou 4gow 2göd g1öf 2g1öl gör4 2g3p2 g4pon g1q 1gr4 gra4 g2rab g1rah
    g1rak 2gral gramm7end 3grap grar3 grau1b grau3f grau3sc grä2 g1räd gräs3c gre2
    2gre. g2reb 2g1rec g1rede 8gredienze 8gredienzi 2g2ree 2g1ref grei4 2g1reic
    g1reih g1reim g1reit g1rek gren2 g1renn grenz5ei grenz3w gres4 g1re3se 4gressu
    gre3st 2gret 2grev 2gria 2grid g3riese 3grif4 grin2 g1ring g2rip gro2 4gro.
    2gro3c gro3i gron4 gro3no g1rose gross5el g2roß gro4u 2grov 3grö 4gröh 3grub
    g2ruf g1rui 6grum 3g2rup 2g1rut 2g1rüc 3g2rün 2g2s gsa4 gs3ab gs1ac gs1ad gs1af
    gs1ag gs1ah g4s3ak gs1ama gs1amb gs3an gs3ar gs1as g3sat gs1ax gs1ä g3sc g4sca
    gsch2 g4schef g4sco gs3d gs1e2 g3see gs2eh g3s2eil gs2eis g3seni gsen2t g4s3er
    g4seu gs3he gs1i g3sic g3sig g3sil gs3inf gs3is g3sit gs3k gski4 g4s3l gs3m gs3n
    gs1o g4sod gsof3 g3sol gs3om g3son g5sorge g3soz gs1p g3spek g4spl g3s2por
    g6sporto gsrat4 gs3s2 gs1tal g3stati gst1au gs1tä g3stel gs3temp gst3err gs3test
    g3steu gs1th gs2thy gs3tier g3stir gs1tis g3sto g4stol gs3ton. g4s1top g4s1tor
    gs1tot g3stö gs1tr gst5reit gst3rit gst3ros g3stun gs1tur gs1tü gs1u gs3un g4sw
    g3sy gs3z 2gt g1ta g1tä g1tec g1tei g1tenn g1tep g1term g1terr g3tex gt1h g1ti
    g1to g3tou g1tr gt2s gt1t g3tu g3tü gu4 3guc g1uf g1uh 1gum gum2e 3gum2m gum2p
    2gun g1unf g3ungl gun2h g3unk gun2n 3g4un2s gun2t gur2 g1url gus4 gus5a 2gusc
    3guß1 3gut gut1a gut3erh gut1h gut3s gut1t gut3z 1gü 2güb gür3c 2g1v 2g1w g2wam
    g2wie 1gyn 2gyn. 2g3z2 2h. ha2 4ha. hab2a hab2e hab2i h1adle 2hae ha5fa haf2e
    h1affä haf3fl haft4sc haft4sp h2aj 4ha3kl hal2 4hal. h1alar halb1r hal3ch 2hale
    halen3t hal4f h2ali 4halla halo1 halo3g 4h1alp hal4te 4haltin 4haltis hal3v
    h1amt 2han. 2han4a h2anbe h2an4d hand1r 2hane han2f hangs1 han2k hank1t han4n
    2hano han4so ha3nu har4 h3arab h1arb 2hari h1arm. h2arme h1arti h2as2 2has.
    2hasa has3k ha3sta has4v h3atl hat4m hat3tr h1audi hauf4li hau5flie h1aufm
    h1aufs h3aug h1aukt haus3a h2ause hau5stei haus3ti hau2t haut3s 4haz hä2 h1äff
    häl3c häl3g här1d här2p h1ärz häs5chen häus1c häus5le 2h1b2 h2bar3a h2barg
    h4barka h2baro h2barr h4bars. h2bart h3bat h2bek h3ben h3bo 2h1c h3ch h2coc
    2h1d2 hde2 hde4k hden2 2he. 2hea he3al he2b he3bar he3bä heb3eis he3bla he3br
    hech2 hed2 he4dem he1det he2el he1eng heere2 he1et he2f hef1ei hef3erm hef3ing
    hef1l hef1r he3fri hehl2 he2i2 h4eib h1eie h1eif h1eig hei4k heim1p 2hein hein3d
    hein3te 2heio heis2 heit2s1 h1eiw 1heiz he2k he3kl he3kr hekt3a he3ku 2hel.
    he2l3a hel1ec he2leg he3lege h1elek hel3ers hel1la hell3au hel2m he2lö 2hels
    he2m 2hema he3mag 1hemd 2heme 1hemm h3emp 2he2n hen1a hen1da hend2s 4hene hen1eb
    hen3end hen1et heng2 4heni henst2 hen3str hen1ta hen1te hen3tie hen3tr h1ents
    h3entw hen1z 2he2o 2hep hept2 he2r 2hera her3ab her3an1 he3rä herb4 her8beste.
    her8besten her8bester her8bestes her3bl her2c he3rec her3eis h1erfo her4for
    her4fri herg2 her4kas h1erke her4kes 2herm 2hern her3ob h1erö her8richt.
    her8richts her6rinn 2hers. her4so hers2t hert2 her1ta her3tr h4erza h1erzä
    her2zö her2z1w hes2 hesen1 hess2 hesse2 heß2 heter2 he3tint 1hetz heu3err heu1g
    3heusc 4heuse he2wi hex2a hex2p 2h3f2 2h1g2 h3ga hge1 h3gel h3gu 2h3h2 h4hold
    hi2 4hi. hi4a2 2hic 2hid h2id2e h1idi hie4b hie3g hie3h hie4r hier3in hier4l
    hies4 hiet4 hi3fa 2hig hi3in hik1t hil3a hil2b hil4d hil4f hilo5 hi4m2 h1impe
    hin2as hin2en hin1ge hin2i hinn2 hin3s hin4so hin2t3a hin2te hin1z 2hio2 2hip
    hip1h hip1i h2ir 2hire hi3ren hirm1a hir4r his2 hi3sp hiß2 hit2a hi3tag hi3th
    hit2l hit2z hitz2e hi3un hi3ur hi3ver h1j 2h1k2 h2kent h2keu h2kir 2hl hlab2
    h1lad hl2ag h1lai hl3an. hlan4d hland3a hl3anz hl1ar h1las h1lat hlau1b h3laub.
    h1laug h3laus. h1laut h1law h3läche h3läd h1länd hl1är h1läs h1läß h1läu hl3b2
    hlber4 hl1c hl1d2 hle3a h1leb h1led hle3e hle3ind hleis2 h3leit hle3l hle3ma
    hl2eng hlen1n hl1erg hl3ernä hle3run hl1erw hle3sc hle3ta hle3tr hle3v h1lex
    hl3f hl3g hlicht6w h2lig h1likö hl1ind h1lini h1list hl1k hl1l hl1n hlo2 h1loc
    hl1of hl1op h2lor h1los. h1losi h1lö h2lös hl3p hl1se hls3ka hl1sku hl3slo
    hls2te hl3str hlt2 hl1ta hl1th hl1ti hl1tr hl1tu h1luf h1luk h3lumpe h1lüf hl3v
    hl1z 2hm h1mad h1mag hma3ge h1man h4manin h1mar h3mas h2mäc h2mäh h2mäl h2me
    hme1e hme1in h3meist hme3la hme3le hme3li h4men. h3mes hme3ta hme3te h3mex h1mil
    h2miß h3mop h1mot hm1p2 hm2s hm3sa hms1p h3mul h1musi h1my 2hn h1nac hnach4b
    hnam4 h1nami h2nar h1nas h1nati hn1äh hn1är h1näs hn1c hnd2 hn1da hn1di hn3dr
    hn2e hne3d hne3g hn3eig hn3ein hne3k hner3ei hn3ersa hn3ex hn3g hnick2 hn1id
    hn3im hn1in2 h1niv hnk2 hn1n h1not hn3se hn1si hn1ta hn3ti hn1to h2nul h1num
    hn1unf hn1z 2ho. ho2b ho2c hoch1 2hod 2ho2e ho3er ho4f hof1a hof3fes hof1o hof1r
    ho1g ho3ge hohen3 2hoi ho2k2 2hol. hol3au hol3b hol3ei hol3g hol3k hol3s 2hols.
    hol3v ho1ly h1olym ho2m2 ho1ma h2on ho1nad hon2d hon3di ho3neu hon1to 2hoo ho1on
    2hop2 ho1pa h1ope ho1ph h1or2an h1ord 2hore hor2f h1or2g hor1ma hor1me hor2n
    ho2s ho3sek hos4g ho3si ho3sk ho3sl ho3sta 2hot. ho1tom 2hot3s2 2hou ho2v ho3ver
    2ho2w how1e h1ox 2hoz 1hö 4hö. höch3l h2ör hör3b hör3f hör3g hör5l hör3m hös1c
    h1öst h3p2 h1q 4hr hr1ac hr3ad h1rat h1raum h1räu hr3b hr1c hr3da hr1di hr2du
    hre2 h1rech h1red h1ref hre3gis hrei2b h1reic hr3eig h1reiz hr1ele h1rep hr4erbe
    hr4erbu hr2erg hr2erh hr2erk hr4erz hres3 hr1eta hr1eu h1rev hr3g2 hr4har hri4
    hrie4 h3riesl hr1ins hr1int h2ris h5ritter hr3k hr3la hr3m2 hr1n hro2 h3rock.
    h1rog h1roh h1rol h2rom h2ron hr1osm h1rou hr3r hr2s1ac hr2s3an hr2sau hr3schm
    hr2s1en hr2s1in hr2s3k hr2s1of3 hr2su hr4sw hr2sz hr1ta hr2tab hr2tan hrt3ri
    hrt4sa hrt2se hr1tu hr2tun hr1ums h1rut h1rü hr3v hr1z hr2zeh hr3zen hr3zu 4hs
    h3sache hs3acht hs1ad h4samt h2san h3sani h2sauf h2säh h4schan hse2 hs1ec h1see
    h1seg h2s1ehr h1sei hs1eie hs1ein h2s1eis hsel1l h1sem h1seri hs1erl hs1ern
    hs1erz h1sess hs1ex h1s2ext h1si h2s1id h2sig h3sign h2s1ing h2sis hs2kal
    h3skand hs3ke hs2ler hs3m h1so h3so. h2s1of hsof3f h2spac h2s1par hs1pel h2s1per
    h2s1ph h2sprä h2spro hs1s2 h2staf hst3alt h3stan hs1tau hst4e h1stec h4steil
    hs3temp h3sterb hs1the h1sti hs1tie h2stit h2s1tor h1str hst3ran hs1tu h1stun
    hs1u 4ht ht1a h1tabl ht4akt. h1tal ht3an. ht3an1e h1tann ht3anz h2tar h1tas
    h2tass ht3at ht3auge h1taum h4tax ht1ä h1tän h1tät ht1ec ht1eff hte3g ht1eh
    h1tei h4t3eilz ht1eim ht1ein ht1eis ht1eke htel2a hte3lie ht3elit htel1l h1tenn
    h1tepp ht3erfü ht3ergr ht1erh ht5erken ht3erkl ht5erleu h3terra ht3erre ht5erspa
    ht3erst ht6erste ht1erz ht1ese ht1ess ht1eu h2t1ex ht1h ht3hen hther2 h1thes
    h1tie h1til ht1im ht1in h1tip h1tis h1tit ht5la h1to htod1 h2tol ht1oly h1tö
    h1tr ht1rak ht3rand h2t1ras h2t1rat ht1rau h4traub h3trec h4tref ht1rei ht1rel
    h2t1res ht3rieg h2t3rin ht1ro h2trol ht2rot h2t1ru h2t1rü hts3an hts3ein ht2sen
    hts3end ht2sp ht4spin ht4stei hts3tem hts2ti ht4s3tur ht4s3tür ht1t2 h1tue h1tum
    h1tur ht1urs h1tus h1tut h1tü ht3w htz4 ht3za ht1zen ht1zer ht1zin hu2 2hua
    hub1a hub3b hub3ei hub1en hub1l hub1r huh1a huh1i huh3m h1uhu 2huk huko3 hukt2
    hu4l hul3a hul1ei hul1er hul1in hul1k hul1l hul1s hul1tu hul3v hul1z hum2a hum2b
    hum2i hum2p h1ums hun2 h1una hun4f hung4 2h1uni 2hur hu4s2 4hus. hus3h huss3a
    hut2c hut2h hut3sc hut4z hutz3er hu3w h2ü hüb4 h3über h3übu hüf2 hüg2 hüpp1 2h3v
    hver2n 2hw2 hwei2 h1weib h1weih h3weil hweis4s hwen2 hwer3b hwer3g hwer3m h1wir
    h2wirr hy2 hydro3 1hymn 1hyp 4hyt 2h1z2 h2zeh h2zen h2zes h2zi h3zie h3zim h3zin
    h3zip h3zir h3zis h3zu 2i. iab2 ia3be ia1ber iab3t iad2 ia3di ia1do ia1f2 ia3g
    i2ago ia1h ia1j ia3ka i3akt ia3ku ial1a ial3as ial3d iale4 ial3erm ial1et i1alia
    ia1lim ial1k ial1l ial3m ia1lo2 ial1se ial1t2 ia1ly ial3z2 iam4 i3amp ia3mü
    ian3alt iand2 ian2e ian3eb ia3net i3ang ia3nor ians1p ianz1t ia1p ia3pi ia1q
    i3ara iard4 iar3r ia3sa ia3sc ia3se ia3sh ia3si ias4k ias4m iast2 ia1str ia2ta
    i3at2h ia1the ia1tro i1au ia3un iaus1 ia1vi ia1w ia1z iä2 i1äm i1äp iät3s4 i3bac
    i3bak i2b1ar ib1auf i3bä i3bea i1bec ib1ei i1bek ibe2l ibe2n iben1a i1beru
    i1besi i1bez ibi4k i1bla i1blä i1blo i1b2lu ib2n i2b1ö i2bra i2b1rä ib1ren
    i2bric i2bro i3brü ib3sa ib2se ib4ste ib3unk ib3unt i3bur ibus1c i4busi i4bussc
    2ic i1car. ice1l ich1a ich1ä ich1ei i2chen ich1l ich2le i3ch4lo ich3m ich1ni
    ich1ra ich3rei ich3rit ich1ru ich2sa ich2s1i ich4spo ich1w i4ci ic5kos icks2
    i1cl ico3b 2id. ida4 i1d2ac idar3b id1au id2d 2ide. i2dec 1idee id1ei ide3k
    2idel idel2ä ide1li ide3me i1denk ide2pa ide1ra ide3sa ide3si ide3so i3deu ide3v
    id2ga id2har i3dik 1idio2 id2lin id4n 1idol. idon3 i1dot id1rin id2sh id2s1p
    ids3tel id1t2 1idy 4ie2 ie3a ie3ba ie5bei ie3bil ieb1l ie3bo ie3bra ie4b1rü
    ieb4sto ie3bü ie3co ied3an ie3e ief1ak ief1an ief3ein ief1f ief2i ie3fid ief1l
    ie3ga ie3gelä ie3gi ieg2li ieg1r ie3gra ieg4s1c ieg4se ieg4st ieh1m ie3ho i1ei
    ie3j ie3ka ie3kr ie3ku iel1a ie3lä iel1d iel1ec iele3v iel3f ie1lie iel1li ie3lo
    ie3lö iels2p iel1sz iel1ta iel1tr ie3lü ie3mana ie3mei iem4h ie3mi ie3mo ie3mu
    ie4mun ien1ag ie3n2am ie3nat ien1da ien3do ien1eb ien4erf ien3g ien1k ien2s4
    ien3sa ien3sc ien3si ien3sk ien3sp ienst5er ienst5rä ien3sz ien3tab ien3tä
    ien3tr ien3za ien1ze ien1zi ie3o ie3p ie4pe iep3to ie3q ier3a ie5raum ie5räu
    ierd2 ie3rei ier3eis ier2er ierf2 ie3rie ie3rit ierk4 ier5la ier3m iero2b ier3r
    iers2e ier4s3eh ier2sk ier3so ier4spe ier3sta ier3ste ier1ta ier1ti ie3rüc ier3v
    ier2ze i2es ie3sc iesen3s ie3so ie3sp ies2s ie3sta ie3str ie3stu iet1a iet3erh
    iet3ert iet1ho i3ethy ie3tie ie3tit iet1o ie4t3ö ie3tr iet1ru iet4se iet3z2
    ie3um ie3un ie3v ie3wi i1ex ie3ze ie3zi 2if i2fad if1an if1arm i2f1au i2fazi
    i1fei if2e4n if1erh i2fex if2f iff2s iff4st if1l if2la if2lä if2le if2lu i1flug
    i1flü if3nu i1fö i1frau if1rä i1fre if3reif if1ru if2s if3sa if3se if3si if3sp
    if3str if2t ift3erk if3tre ift3ri ift3sp ifts2t ift3sz i1fu 2ig ig2ab iga3g
    ig1ang ig1art iga3s i2g1au ige3ber i3gefä ige1g i1gei ig3eise i1gel i2gelt
    i1gema ige3me ige3mo i2gen ige2na i3geno i3geric ig1erz i3gesc i3gese i1gesp
    i3gew ig2fr ig1im ig1l i1gla ig2le ig3lein ig3ler ig2mu ig2na ig2ne ig2nu i3go2n
    i4gona ig2ra i4graf. ig1rei ig3sa igs2ag ig4schr ig3so ig3sp ig4spa ig4s3pi
    ig3stei ig4s1to ig4stö ig3str ig4stre ig3stu ig3s2tü igung4 i2gü i1gy 2i1h
    i4h1am i4h1ar i3hä i2he ihe1e i3hef i3hei i3hel i3hera ihe3re i3her2r ih1l ih1m
    ih1n i3ho ih1r ih1ti i2h1um i2hung ih1w ii3h i1im i1in i1j i2ji i3jü 2i2k i4kab
    i4k1ak ik3amt ik1ano ik1ar ika5sc ik3att ik1au i3kä i4k1är ike2 ik1ei i3kenn
    ik1ere ik1erf ik1erh ik1erl ik1eta i3kic i3kil ik1in ik1l i3k2lä ik3lei ik3lo
    ik3lö ik1n iko1b ik2op ik1orc ikot1t ik1q ik1ra ik1rä ik1re i3kri ik1s ik3sa
    iks2z ik1tab ikt2e ikt3erk ikt1r ik1tü i4kum i1lab il1ac ila3f i1lag il1ak
    il1ama il1ans il1asp il1au i3l4aufb 2ilb2 il2d il3dat ild3ent ild2er il3di ild1o
    ild1r 2ile i1leb il1ec i3lehr i1lei ileid4 il1ein il1el ile3li ile3na il1ent
    il1erf il1erg il1erh il1err i2les ile3se i1leu i1lev il3fa il2fi il2fl ilf1le
    ilf1re ilf4s ilg2a il3ges il2gl il3gr il2he i4lichs ilien1 i3lif ilig3ab ilik4
    il1ind i3linie i1link il1ipp il2j il1k ill2a ill4an ill2e il1lei il2ler
    ill8fahre ill4fr ill2k il3lu il2m ilm3at ilm1au il3mi il1n il3ne 2ilo ilo3b
    ilo1he ilo1k i2l1or ilot3se ilo1w i1lö il3p il1ser ilt2 il1ta il1th il2ti il3tie
    il2to i1luf ilung4 il1ur ilz3erk i1mac ima3d imal3m i1man im1arc im1arm im2as
    ima3sc im4at imat3s i1mau ima3z imär3s im3ber 1im2bi im2bo im1d 4ime i1med imel2
    im1ele i2me2r im1erf imer2s im1erz ime3tr im1ex 2imi im1inf im1ins imi3se 4imm.
    im2man im4me imm3ent im1mis 1im3mo im4mod 4immr 4imms 4immt im1n i2mog im1op
    im1org i2m3ö im2pal imp2f im3pla 1im3po 1im3pu im4ro im2se im3ser im2so im3sph
    im1ti imt3s 2imu im2um i1my i2n 2in. 2ina in1ac in4acht in1ad in4a1f in4alp
    in1am i3namen in2ant ina3p i4nar ina3res ina3sc in1asi in3au in1äh in1äs in1äu
    2in1c 2indä in2deb in2dep indes4t 1in3dex in3dic 1indiz in1do in2dol ind3sp
    1indus in3d2ü 4i4ne in1ebe ine3d in1ehe in1eng ine3p in3erbe in3erbi in2erh
    in3ertr in2et in1eu ine3un inf4 in3fl 1info. 1infos in2fra ing1af ing1ag ing1ar
    ing4sam ings3pr ing5stä 1inhab 2inhar 2inhau 2inhe ini3b 2inie 2inig in4ir
    2inis2 ini5se in1it ini3tr 3inkarn in2kav ink3ent ink3tie ink3tis ink2ü 2inn.
    in2nen inn3erm in2nes in2net 2innl inn4sta 1innta 2ino ino1b ino1n in1or ino1se
    ino1st ino3ste ino3t ino3v ino1w in1öd 2inp 2ins. ins2am ins3aug 2insä insch2
    inse2 4insed in3seli in3sem 2insen in1si 2insk in2ski in1skr in3spe 3instal
    in4s3tät 4ins2te ins2ti inst4ma in3str in2su 1insuf 2insy in1s2z 2in1ta in3tä
    1integ in3tent in2ter in1th in1tie in1to int3st in3tu in1u4 in2um in3unz 2inver
    2inw in3zem in2zer in3zu io3a io1c i3od io1da io3e iof2l io1g io3h io1k io2kl
    io1m i2on2 io1nac ion3au ion3d ionen1 io3nik ion3k ion3n ions1 ions3a ions3p
    ion4spi ion4stä ion3t ion3z io1p i5opf io1q 2ior ior1c ior2g io1rö ios2 io3sat
    io2se io3sens io1sh io1st iot2 io1te ioten1 io3to io1tr io1w i3ox iön1 i1pa
    ip2an i3par i1pä i4pel iph2 i2plem i2plen ip4m i1po ipo1c i3pol ip2p 2ipp.
    2ippik 2ippis 2ippo ip3pr 2ipps 2ippu i1pr ir2 2ir4a4 i1rad 3irak i1rau i1raz
    2i1rä ir3äh ir3bu 2irek ire1t ir4g irg4s3 2irig 2ir4k irk3l ir4l irm1au irm1ei
    2irn irn3ers i1roc i1roh 1iron iro4s i1rö ir3ra ir4re3ge ir3ri irs2 irsch3m
    irsch3r irsch3w ir3sh ir3sin irt2s1t 2iru irus1 i2rut 2i2s i3sac i4sach i4sam.
    isam3b is1amt is3are is1än 4isc isch3eh isch3ei isch1l isch1m isch3ob isch3re
    isch3ru isch3wu i4s1cr ise1b ise1c ise1e ise1g iseh2 i3sei ise3inf i3sekü i4sel
    ise3lit i3seme i4sen isen1k is1erm i4ses is1ess i3set i4set. is3etat is2eu
    is3har is3ho is1id isie4 i3siev i3sil i3sim i3sinn isi3st is3kel is3ker is3kr
    is6kusse is2ler is2me is3met is2mo is3na is1of 3isol is1op is1org 3isot is1pa
    is2per is3pic i4spl is1po is3pres is1pu is4saa iss1ac is3sai is2sau is2säl
    is4s3che is1see is3senk isser4t iss3lu is1soc is1sor is1soz is4s3per is4stec
    iss1tr is4stri is4stro is4sub iss1um ist3ac is2tal is3tang ist2e4 i3stel is1th
    is2ti i1stic i3stil i3stö istör3 is3tras ist3rei is1trü is1tur i3stü i3suf
    isum1p is3urt i3sy is2z is3ze iß1ers ißler3 iß3lu iß3ri 2it. ita2 it1ab. ital1a
    ital3l it1alt it1an it1ar it3are it1au i3tauc i4t1ax 4itä i2tägl it1äs it4dem
    2ite i1tec ite3g it1ei it2eil 4i2tel itele4 ite3leg ite3li i3tetr it1he it3hei
    it1hil itho8logische iti2 it1id itik2e it1in it2inn ition4 it3iss itler4s it3lu
    it3m ito1c i1tod it1of3 i3tonu 2i1tr it1raf it1rah it1ran it1ras it1rau it1räu
    it1re i4tref i2tro it1rom it1run it2sa its1ag it2s1e its3er1 its3tem it2ta it2tä
    itt3hä it1tra it3tro itt2sp it1uh it1um i3tül it3wo i2ty1 2itz it2zä it2zer
    itz3erg it1zin it2z1w 4i1u4 i2um ium1i ium3n ium1pe iur2g ius1 i1ü iv1ak ival1l
    iv1ang iv1elt iv1ene iv1ent iv1erh iv1erl i2ves ivil1l iv1re iv1t iv1ur 2i1w
    i3we i3wo2 2ix 2iz i2za iz1an iz1ap iz1au ize1c iz2ei izei1c izei3g iz1ene
    i1zent izen2z i2zer iz4erh iz1erl iz1ir i2zis iz3n iz1ob iz1ö iz4sä iz1th i2z1w
    iz2z 2j. 2ja. 2ja3b 3jag jahr3ei jahr4s3 ja3j jal2 2jami 2jan2 janit2 ja3sa
    ja3sc ja3st 2jau. 3jä jäh1l je2 2je. jean4 jed2 jel2 je4p 2je4s2 jet1a jet1h
    jet1r jet3st jet1u 2jew ji4 2jia jo4 2jo. job1 joh4 2jol jon2 jord2 joy3 2js
    j2u2 juden3 jugend3 jum2 jun2 jung1r jung3s jur4 jut2 jute1 jü2 2j2v 4k. 4ka.
    ka2an ka1ar kab4b 2k1abo kab2r k1abt kada4 2kade. 4kaden. 4kadenz 2kadi k3adr
    2kae kaf2 2ka1fe ka1fl ka1fr k2ag ka1ha ka4i kai3sc ka3kl 2kal. 4kala kal1d
    kal3eri 2kalg kal2k k1allt kal4o ka1log 2kals kal1se kal1tr kam4 3kamer 3kamm
    k2amt 4kan. 1kana kan3as 3kanä kan2d4 4kanda 2kane k1ang kan3k2 2kanl 2k1anna
    2k1ans kan2t kanz4l kanz1t ka2o1 3kap 2kara. 2karas k1arbe k1arc kard2a ka1rei
    kar4fu k2ar4g k2ar3k 2karm kar3p kar4pf k2ars kart2 k2arta kar3tal 2k1arti
    kar4wen 1karz ka4s4 4kas. ka5so kass2 ka2t 2kat. kat1an 2kati 2katm kat3se kau2
    kau4f kauf1o kauf4sp kaufs5te kau3g k2aus. kau3ta kau3tr ka2w 2kay 1kä2 k1ämi
    käs3c 2k1b2 k3be1 2k3c kcon2 2k1d k3dä kde2 4ke. ke1b keber2 ke2da kee2 2kef
    1ke2g 2ke3ga kege2 ke3gr 3keh2 ke1her kehr2s kehrs3o 3kei k1eic 4k1eig kei2l
    4kein ke1ind 4keis k1eise keit2 keits1 ke2l 2kel. kel1a kel2ag kel3b2 keld4
    4kele k1elek kel1er kel3g kell4 kel1la 2keln kel1o 2kels kel1sk kel1ta kel1tr
    kel1z k1emp ke2n 4ken. 2ken3a ken1da ken3dr 1kenn ken1ni 3kennt 2kenr ken1s2k
    kens4te ken1sz k3ente. ken1th ken3tr k1ents ken3z keo2 4kep ke2r 4ker. ker3f
    k3ergeb ke3ring kerk4 k2erko k3erleb kern3eu k1ero ker1oo ker3r kerz2 k1erz.
    k1erzi ke2se 1keß 4ket. ket1a ket2c ket1eb keten3t kett1h 1keu2 ke1up k1ex 2k1f
    2k1g2 kge1 k2gos 2k1h k2hac k2ham k2hart k3he k4hee 4ki. 2ki3a ki4ad ki1be ki1bu
    ki1da 2k1ide ki1di 2kie4 kiel3o kies3s ki1f2 2kig ki3h 2ki3k4 1kil2 2ki1la ki1lä
    ki1le ki1li kilo3 2kim ki1ma ki1mi ki3nat kinde2 1kine4 2k2ing 2kinh k2ini k2inn
    kin2o 4kins k1inse k1inst 4k1int kio2 ki1or ki2pe ki1pi 3kir ki1ra ki1re 4kiri
    2kis. ki3sa 2kisc ki3si ki3s2p ki3sta 2kistl kit2 ki1te kitz2l ki1v2 4kiz ki1ze
    2k1j 2k1k2 kke4 k4kei k3kor k2kub k2kug kl4 4kl. k2la1b 4klac k2lam klan2 1klar
    3klas k1last klau1d k1laug klaus4t klär3c kle4 4kle. k1led k1leg 2kleh k4leid
    k1leit k2len 2kler k2les 2k1leu k1lie 2klig 3klim klin2 3klina k1liz klo3br
    k1loc k3lok klo3sc klung4 k1lüc 2k1m 1kn4 3knab kne4 knie3g kno4 3knö k1num
    3k2nü 1ko 4ko. ko3ad ko2ba ko2bl ko2bo kob4s 3koc ko2d ko3e kof2 k1ofe koff4
    ko2g ko3ge ko3gr koh2 2koho ko2j ko2k2 2koka ko3kan ko1ki ko2l 4ko3le kol4k3
    kol4m ko3log 2kom. ko2me kom2i kom4mas kom3mer 3kommt 2ko2mo kom2t k2on 2kon.
    kon1d kon2e kon3s4 kon3tu 3konz kop2 4kop. 2kopa kopf3en kopf5err 2ko1ph 4kor2a
    kor4b kor5bac kor4d 4kori kor4k kor2m korn1a kor4p 2korpi kor2s 2kor2t k2o2s
    2kos. 2ko3sc ko5sin kos1p kos2t ko2ta 4koth ko1tr kot3sp 4k1ou ko3un 4kov ko3va
    ko2vi 2kow 2k1ox 2koz 3kö köf2 köl2 kör4p 2k1p2 kpo2 2kq k1qua 1kr4 4k1rad k2raf
    2krai k1rats krau2 k1raum 2k2raz k2räc k2rän k1räum 4kre. 3kreb4 4k1rec 2krede
    2kref 4kreg krei4 k1reic k1reif k1reih 3kre2m kre3mi 4kren 2k1rh k2ri4 2krib
    2k3ric k3ries 2krip 2krol k2ron kro3nes kro1p kro3sc 2krot 4k1rou kru4 krum2 2ks
    k2sal k4s1amt k2san ks4ana k3sand k4sar k2sau k3sau. k3saue k2s1äl ks2än ksch2
    ks1eb k1seid ks2eif ks1ein k1sel ks2end k2sent k1seq k1seri ks1erl ks1ers kser2v
    ks1erw ks1ex k2sha k1shi k1si k2s1id k2s1in ks3ke ks3m ks3n ks1o k3son k2sor
    k2sö ks1pa k2s3pan ks2pat k3spe ks3s2 kst4 k2s1tal k4s3tanz k1ste k4steil k2stet
    k3steu k1sti ks1tie ks1tis k2s1tor k1str k2strä k4strop k1stu k2s1tum k2stüt
    ks1u ks3un 4kt kt1ad k2tak kt1akt kt1am kt1an kta1p kt1ar kt3are kt3ars k1tas
    ktat3t kt3au kta2v k1tä k2tea k1tec kt1ei k1tel kte3li kt3erfo kt1erg kt1erh
    kte3ric kte1ru k2tex kt1h kti2 kt1id ktien3 kt1ing kt1ins ktion4 k1tit k1toc
    kt1of3 kto1l kt1ope kt3orga ktor3k k2t1ras kt1rau k4tref kt1res k2tric ktro3m
    ktro3s kt1run kt1rü kt3ser kt3s2t kt1su kt1s2z kt1t2 ktur3t kt3w kt3z ku2 4ku.
    ku3be 2kud4 ku3dr 1kug2 kuge2l 1kuh k1uhr 2kui kul2 2kule 2kulin 2kulo 2kulö
    4kulp 2kulu k3uml kum2p kum2sp kun2 k2una 1kunf 2kung4 kun4m kun4s kunst3 4k1up.
    1kur2 kur3ans kur3ei kur3g kur3m 3kurs kur4sp kur4str 2kus. 2kuso 2kusr 4kusse.
    4kussen 3kussiv 2kusv 2kusw 1kü 2kül 2küne kür4s 3kürz 2k3v 4k1w 2ky2 ky3li ky3m
    2k1z2 k3zu 4l. la2 4la. la3ab l2ab. 2labb la3bew l1abl 2labn lab2o 7laborations
    labor3t l2ab2r la3bru 2labs la3bur 2labw 2labz 2lach. l1ada lad2i l1adl 2lad2m
    la3do 2l3adr 1l2adu l1adv 2laf laf3fes laf3ta la4g lag3eis 2lagg lag2m lag3ma
    lag1ob lag3se lag5ser 1lahm 2laho lais1t 2lak2 lak3i l2akk la3kl lak3tu 2l1al2
    lame3s 2lami 1lammf l2amp l1amt lamt2s 4lan. la3nac l1anal 2lanc 1l2an2d land1a
    land3au land5erw land5erz land5inn land3l 4lane 2l1anf langs2 lang3se lang3si
    lang3sp 2l1anh l2anhe 4lanl 2lanm 4l3ann 2lano l1anp 4lans l1ansä l3ansi lan3sp
    2lant lan3tel 4lanw 4lanz lan2z1w la3or lap2 2lapa 2l1apf l1apo la3pr 2lar
    lar3an l1arb lar5ba l1arc lar3ei lar3ene lar3f l2arg4 lar3ini lar3l lar3n lar3re
    lar3sc lar3st l1art lar3ta lar3th l3arti la1ru la3rus lar3ze las2e 2lash 2la4sp
    5lasseri 5lassern 5lassers 2lassü la4st las2to las3tur 4lasu 1laß laß1th lat2e
    l3ath 2latin 2latm lat2o lat1ra latt3in latt1r lat4z 4lau. lau2b1r lau2f lauf1i
    lau4fo lau1h lau1ma 1laun l2aus. l1ausg l1ausl l1ausr l1auss l1ausz laut3sc 2lav
    la3vo 2lawe la4z lä2 lä3an 2läf 2l1ähn l1ämt länd3l 2länz lär3b lär2c lär3k
    lärm1a lär3sc lär2t l1ärz läs3c 2läse4 2lät 2läub 2läuc 2läue l1äug 2l1b l3bac
    lb1af lb1ang lb1arb lbdi3 lb1ede l3beg lb3eink lb3eise l2bem l3bep l3best l3bet
    lb1eta l3bez lbi4 lb1id lb1ins lb2lat l3blä lb1le l2b1li lb2lo lb2lu lb1ohn
    l2bou lb1öl l2brec lb1rit lb2s lb3sa lb3se lb3si lb3so lb3sp lbs4t lbst1e lb4sto
    l3bud lb3uf l2bum l3bü 2lc l1ca lch1le lch1li lch3m lch1n lcho4 lch1r lch1se
    lch3ü lch1w lch3z l4ci l1cl l2cu 2ld ld3ab l1d2ac ld3ack ld1ad lda4g ld1ak ld1al
    l1dam ld1amm ld1an1 ld3ane ld1ar ld3ari l3darl ld1au ld1är ld2bre ld3d lde3g
    lde4h ld1ei l1del ld1ele lde3lis l1dem ld1emi l3depo ld2erl ld1erp lde5sa
    ld1e2se l1dez l1dia l1did l3dien l1dif l4din l3dio l1dis ld3l ld2n ld3nu l1do
    l2dom l1dö ld1ra ld1rä ld1re ld3rea ld1ri ld1rö l3dru l2ds ld3s4a ld3ska ld3spa
    ld3s2t ld1t2 ld3tu l1due ld1um le2 4le. le3ak lear3t 1leas le3ba leben2 2lech
    lech1a lecht4e le3die 2lee. le3ei 2lees 2lef4 le3fe le3fl leg1ab leg3as lege1b
    lege3h leg2g le3gr 1leh le3her 2lehs 2leht l2ei. lei2b lei3bel l2eic l2eid
    2leien 2l1eig2 lei2h l2ein. l2eind l2eine 2leinf l1einn l2einu le4is leiss5er
    l2eit leit7ersc leit3st le3kr 2lel l4ela le3lag le3lä l1elek l2eli lel1tr 4lem
    le3mal le6mark. le4mä lem3br le3met lem1o lem4p lem3st 4len. 2lena 4lenb 4lend
    lend2r len3dro 4lene len1ed len3end len1eu l1engl 4lenh len2kl len3kli 4lenm
    le3nov 4lenp 4lenr 4lens len1si len3ska len1sz 4lent len3tal len3tr l1ents
    l3entw 4lenw l1enzy le3of leo2m leo2p le3pa l1epi 4leptis l2era ler3bu l3ereig
    ler3eim ler3eis ler3f l1erfo l2erfr l2erfü l3ergeb ler5gu 2lerik l2erka l2erko
    l4erlei 2lernä l4erne 2l1erö l2erra lers2k ler3ste le3rüc ler3v l4erwa l1erz
    2les. les2am le3san les2bi les2e le3s2h le3s2k les2n les1po 2lesse lest4 le3sto
    le3stö le3str les2z 4leth let2r 4lets leu2 2leud le3uf l1eul l1euro 1leut le3vis
    le3vo 4ley le3z 2lé 2l1f l3fah l6fahret lfang3 l2fast l2fe lf1ec lfe1e lfe1g
    l3fei lf3einh lf1eis l3fel lfe3le lfe1m lf2en l3fes lfe3sc l3feu lf1f l3flä
    lf1led lf1lo l3flu lf2sa lf2s1e lf2s1pe lfs3tau lf2s1ti lfs1tr lf2su lf2tr lf1tu
    l3fu 2l1g lga3d l2gam lg1art l3gau l2gäu lge2br l3gef l3gei lgen2a l4gers l2geti
    l2gi lg1lo l2go lgo1l lg4p l3g2ro 2l1h2 l3ha l4hard l3hei l3her lher2b li2 4li.
    2l2ia li3ak li4at li3bal 1lic 4lica 2lick l1ido lie4 4lie. liebe4s 1lied 3liefer
    2lien lien3s 2lierz lies1c 4lieu li4f4 2lifec 1lift. 2lig. li3gei li4gre 2ligs2
    2lii li4k 2lika li5kli 4liko lik2sp lil2 li3lan li3lau li3lit lim2a lima3g li3me
    lim2m 1limo 4lin. 4lina lin1al lin2ar lin2d 2linda lind2b 2l1indu 4l1inf 2lingh
    2l1inh lini4 4lini. l1init l3inj link2s 2lino lin2q l2insa l2insc l3insel l1inst
    2l1int lin3tes 4l1inv 4linw lin2zo 4lio li3pf li3pr 1liq lis2 4lis. 2lisa li3sal
    2lisc li3scho li3sek 2lisi 2lisk l3isl l3iso liss2 lit2a li3tag li4te 3lith
    lit2s lit3sa lit3st lit3sz 2lity lit4z 4litz. 2liu 2liv liv2e li3w 2lix li4z
    lizei3t 2l1j 4lk l1ka lk1alp lk2an lk1arm l1kee l1kei l1kess l3ket l1ki l3kit
    l1kl lk1lad lk2me l3kol lkorb1 lko2v l3k2ra l2k1ru lk2s1 lk3sä lk4stä lk1tü
    l3kur lk2ü l2kür lk2wa 4ll ll1abb ll3aben ll1abt ll3acht ll1aff lla3kr ll2al
    ll1am llan2t ll2anw ll3anz ll1arm ll3aufg ll3aufk ll1aus ll3b2 ll1c ll3d2
    lle3ber l1lec ll1ech lle3d ll1ef ll1eim ll3eise llen1a ll3endl llen3dr ll3endu
    ll2eng llen5tes lle4r ller1d ller6gen ll3ernt ll3ertr ll3f2 llg2 ll3gi ll3go
    l1lib l1lief lli3gr llik2 ll1imp ll1ind lli3r l1litz l3lize ll1k ll3kr ll1l2
    llm2 ll3mo lln2 ll1ob ll1of ll1ol ll1opf l2l1or llo2t ll1ou l1lö ll3p ll3sä
    ll1se ll1sh ll1s2k ll2spr ll3spre llt2 ll1ta ll1th ll1ti ll1tr llt3s ll1tu l4lun
    ll1urs ll3v ll4wit l2ly l3lym ll1z2 llzu3ge 2l1m lma2 l3mac l3mag lm1aka lm1am
    l3mani lm1apf lm1arc lm1art l3mä lm1äp lm1äst lm1c lm1d l3med lm2ei l3mel
    lmen4sc lm1e2p l2mer lm1erz lmin2 lm1ind lm1ins l2mis lm1m lm1n l3mom lm1orc
    l3mö l4möl lm1p lmpf4 lm3se lm1st lm3ste lm1s2z lm1th lm1ti l3mul l3mus 2ln l1na
    lnar4 ln3are l1nä lnd2 l1n2e l2nen l3no l1nu l1nü 4lo. lo2b 3lob. l1ober 2lobl
    l2obr lo2di 2loe 2lof2 l1ofe lof3fe lo1fl lo4fo 3logi 6logiert lo2gn log3te
    loh2e 2l1ohr 1lok lok3r 3loks lol4 lo1la l1oly 2lom lo2mä lom4b lo2mi lo2mo
    lo1mu 2l2o4n lon2n lon1o lo2o 4lop2 lo3pa lo2po lo3pol lo2r 3lorb l1or4d l1or2g
    lor1m lor3ni lor3p lor2q 1lo2se lo3sec lo1shi los3ka los3m loss2e los3ste los1to
    los1t2r 2loß lo1tä lo2te lo2th lo1tr lot2s lotz3k lo2v lo3vo 2low 1loy 1löch
    2löck 1löf 2l1öfe l1öhr 2l1öl löl3b l2ölu lör4 1lösc 1lösu 2löß 1löw 2l1p l3par
    lpar4k l2pe l3peg l3pes lp4f l2ph l3phä l3phi lp1ho l3phr l3pht l3phy l2pi
    lpin3s lp4m lpo2 l3pot l3prei l3pro lpt2 lp1tr l3q 2l1r2 lrat4s l4raun l3rh
    l3ro2 lrut4 l3rü 4ls lsa2 ls2ac ls1ad l3s2al ls1amb l2sanf l2sang l2sann l3sat
    l2sau ls3aug ls2äm l4schen lse2 ls1eb ls1ec l1see l1sei ls1ein l2s1em ls1ere
    ls1erg ls1erl ls1ers lser2v ls1erw l1sex l2sha ls2hi l1si l2s1id l2s1imp l2sind
    ls2kal ls3ke l2s3ko l4sky ls2log l1so ls3ohne ls3ort. l3sos l2spac l2spun ls1s2
    lst2a lstab4 ls3tabl ls2taf l2s1tal ls1tas l4s3täti l1stec l3stel l4sten l3steu
    l1sti ls1tis l2stit ls1ton ls1tor l1s2tr l3stra l1stu l1su ls1um l2sun l3sy 2lt
    l1taf lta4g4 lt3agr lt1ak ltal2 lt1am lt1ang l1tann l3tari l1tas lt3ato lt1au
    l1tä ltde2 l1tec lte3g lt1eh l1tei lt1ein lt1eis l1tel lte1mi l1temp lt2en
    l1tenn l1teno lten3tr lter3a lt3erde lt2erg lte3rie l3term lter2n lt2erö lt1esk
    lte3str l1teu lt1eur lt1h lt2hu lt3hun l1tic lti3k lti1l lti3na lti1r lti3u
    lti3vis lt3l l1to lt1of lt3org lt1ori l2tos l3tou l2tow l1tö lt1öl lt1ös lt1öt
    ltra1l l6trans. l3trap lt3raum lt1räu lt1re lt3ris lt1ro lt1rö lt1ruc lt2sei
    lt2spu lt4s3tab lt4stec lt1t l1tug lt1uh lt1um ltur3z l1tü lt3w lt1z lu2 2lu.
    2lub1 lu3be lub3sz lub3u luch2 luck2s lu3do luf4 2l1ufe luft1 lug1eb lug3erp
    lug3g lug3l lug1n lug1r lug3sa lug3sp l1uh lu3lö lum2b l1umj l3umk lum2n l1ums
    l1umw 2lun l1una l3unf lun2g 4lung. lung4sc l1uni lun2k 2luo lu3pl lu3rä l1urn
    l1ur1t lu4s2 2lus. 4lusc lus3k lus3p lus4s luss1c luss3or luss3p 1lust lust3a
    lust3re lus3u 2luß1 lu4t lut1a lut3erg lut1of lut1or lut1r lut5schl lut1t lut3ze
    luz2 2lübe lüh1l lüt2 2l1v2 l2va l2ves l2vi l3vil l3vl l3vo 4l3w l4wang 2lx 2ly.
    ly1a ly1ch ly1et ly1l 2lym ly1me ly1mo lyn2 1lync ly1ne 1lysis ly3th ly1u 2lz
    lza2 l4zac l2z1ap lz1ar lz1aus l2z1äp l2z1är lze2 l3zei l1zel lz1ele l1zent
    lz1erz l3zett l1zi lz3l lz1of lzt2 lz1tep lz1th lz1ti lz1u2fe lzug2 lz1w 4m.
    4ma. 2maa4 2m1ab ma3bal ma3ber ma3bi ma3bli m2a3b2r ma3bü mach2t m1adm mad2s
    ma1en ma1er ma3fe ma1fr ma1fu mag2 ma3gar 2magg 2mago ma3gor ma3gr mah4 ma1han
    ma1he mai1d mai5sone mai1v 3maj mak2 ma3kar ma3kat ma3kon m1akt mal1ak mal3at
    2mal3b m1ald mal1da mal2e 2mal1k mal1li m4al3p 2malq mal1ta mal3ut mal3va 2malw
    mana2 1manag m3anal man2d2 man3ers mang2 m1angs 2manin 2mank man2n 1mann. 2mano
    1manö mant3s 4manz 2mao ma1pf 2ma1ph ma3pl 2ma1pr 4mar. 4marag ma1rah m1arb
    4marc 2mare ma3rest mar4g2 3m2ark 4markl mar4li marm2 2mar4o 2marr mar4sp 1marx
    mar4z 1mas2 4mas. ma3schm 2mase 2ma3sp mas4q ma1str 2masy 1maß mat3erd mat3l
    4mato mats2 mat3se mat3sp mat3st mat3tag mat3tal mat3tr mat3url mau4 1maue m1auf
    1maul ma3un 2mausa maus2e ma1v ma1w max2 may1t 2ma2z ma4ze 1mä2 2mä. 2m1ähn 4mäi
    4m1änd 2mäo 4mäp mär3g 3mäs 3mäß mät4 mäu4 mäus1c 2m1b2 m2bab m3bac m3ban mba1p
    m3bat m3bau m3bä mbe2e m3beg m3beh m3bei m2beke mben1t m3bere m3bet m3bew m3bez
    m4bik m3bil m4bit m4bi3z mble3i mbol3l m3bö m4bra. m4bris m3bru m3brü m3bun
    m3bur m3but 2mc m3ca mc3ca m3ch m1cr 2md m1d1a md1ä mde2 m3def md1ei m1di mdien4
    md3le m1do m3dru mds2e md1um md3w 4me. me3a me3b2 mecht2 me2d 3medi me1ef mee4n
    me1ene mee1ru me1g mega3s 3meh 1mei2 m1eif m1eig meil2 mein4d mein6har m2eist
    me2l me3lag mel1au me3lä 3mel2d m3elek melt2 mel1ta m5eltern mel1tr mel2z mem4
    me3me m1emi m1emp me2n 2men. men3ar men3au 2mend m1endl 2mene 2menh men1k 2menl
    2men3r m4ens men2ta men3tis m1entn men3tr men1z 4meo me1ou m1epi me1q me4ra
    mera3g mera5lin mer3d mer3f m3ergän mer2k 2mers merz4en mer2zi mer4zu. 2mes.
    4me2sa me2sä 2mesb mes2e me3ser me1sh me2si 3mes2s mes4sa mes4sä mess1o mess1p
    2mest 3meß1 meß4ban meß8buch. meß8buchs meß4kun meß8platz. meß4rau meß6schn
    meß3u 1m2e2t2 2met. me3taf meta5g me3tar meta3s me3tau 2meti me3tim 3metro 1meu
    mex1t 2m3f mfahr4t 2m3g2 mga4 mge1 mge2he mge2hu mge3l 2m3h mher2r 2mi. 2mia
    mi1ar 2mid mi2e 2mie. mie3b mie3c mie3dr mie3i mie3l mie3no mie3rat mie3rec
    mier3z mie4t miet1i mie4z 2mi2g 4mik. mi3ko mil4 3milc milch1 mild4s mi1leg
    1mill 2milz mi1ma4 m1im2m 2m1imp 4min. 4mina 4minb 3mind. 4minf 4ming min2ga
    min1ge ming2h ming3st ming2w 2mini. min2o min3of 1minz 2mio mi1os mi1p mi1q 2mir
    mi1re mir3sc mis2 mis3a misch3w mi3so mi3sp mis4s 1miß miß1e 1m2i2t mit1ak 2mitä
    mit1er mit1es 5mitg mit3h 2miti mit3s2 mit5sa mit1so mit1ta mit3tan mit1u 2mitz
    2mi2u 3mix miz2 mi1ze 2m1j 2m3k2 mkin2 2m1l2 m3la ml3c m3leb m3leh m3lei m4let
    m4lig m2lis m3list m3lo ml3p ml3sp m2lu 4mm m1ma mma3a mm1ak mm1al mm1ang mm1ans
    mm1anz mm1art mma3st m2mata mm1au mm1d2 mm1ein mm3eise mmel1l m2mens mmer3a
    mme2re mmer3k mmes3a mm1eu mmie4 mmi1m mm1inb mm1inf mm1inh mm1ins mm1int mmi1r
    mmi3sc mmi3st mm2le mm1m mm1na mm3ni mm2nu m1mo mmo2p mm1ope mm3ö mm1p mmpf4
    mms2 mm2se mm1ti m1mu 2mn2 m3nac m1nä mne4 m2nen m1ni mni1p m1no m1nö m1nu 2mo.
    2moa 1mob 1mo2c 3m2o2d mode5ric 4mody 2moe mo2f 2mog. mog3al moge2 mo1gl 1mo2h
    mo2i 2mok mok2k mo4kri m2ol 1mom2 mo3ma mo4mi m2on2 2mon. mo1nau 4mone mon3er
    8monnaies mons2 mon3sa mon3th mo1ny 1mo2o 2mop mo2pa mo1ph mor2 1mora mor3ar
    mor4d mord3a mor3f mor3l mor4r mor3tr mor3z 2mos mo2ska 4mosp mos3s mos2t mo3sta
    moster4 mot2 mo2v mo3ve mo3w 2m1ox 1m2ö 4mö. möd2l 3mög 2mök 2möl mör3f 2mp m1pa
    mpan2 m2paz m1pä m2ped m2pel mpel4mu mpf3erg mpf3erp mpf3err mpf3erz mp2f3l
    mpf1or mp1hos m2plu m1po mpo1m m3pr mp3ta m1q 2m1r2 mre1b mre2g m3ro2 m2rö 4ms
    m2san ms3and ms3ap ms1as m3sat m2sau m3sä msch2 m1se2 m3see ms1ef m3sek ms1ene
    m2ser ms1erf ms1ex m1si m3sic ms1ini m3sit ms3ke m3s2ki ms3ko ms3m m1so m3sol
    ms1ori ms1ped m2spot m2spro ms1s mst2 m4stag ms1tal m1stec m3steng m3sti m1str
    m1stu m3stü m1su ms1ums m2sun m2sü m3süc m3sy 4mt m1ta mt1ab mt1ak mt2am mt1ar
    mt3are m1tä m3tea m1tec m1tei m3teil mt1ein m1tel mt1elt m1temp m1tep mt1erf
    mt1erg mt1erl mt1ers mt1ert mt1eta mt1eu mt1h mt3ho mt1im mt1ins mt1int m3tit
    mt2ne m1to m1tö mt1öl mt1ös m1tr mt1ro mt1rö m4ts mt2sa mts3chi mt3sco mt2s1e
    mt1s4ka mts1p mt2spr mt4s3tät mt1t m1tu mt1um mt1urt m1tü mt3w mt3z mu4 2mud
    3muf2 2mug 2m1uh 2mul2 mult2 m4um4b mum2s1p m1unf mung4 m3ungeb 2mungs m2unr
    mun3th mur2ma mur1uf mus1a 1musc 4musen mus2k mus3kr mus1o 2musp mus3to muße3
    mut1au 1muts mut3sc mut4str 2mutt. 1mü 2müb 2müc 3müd 3müh mühl1a 3mül 3mün4
    2m1v m3ve m3vo 2m3w mwelt3 my2 2my. my3kl my3la myn2 myo3 my4s 2m3z mzin2 4n.
    4na. na2a 2n1ab n3abd na3ber na3bes na3bru 4n3abs n3abu nach6bar 3nachm nach1s
    3nachw 2nada n1add 2nado n1adr 2nae na1es 2n3af 3nah nah1a n4ahm n1ahn nai2
    n3air 2n1ak na3ko na3kr n3akt na3ku 2nal nal1a nal3am n2al1d nale4 nal3ent
    nal1et n2ali nal1ku nal1la nal3lei nal3m na4lo nal1se nal1t2 na2m 2nama nama3z
    2namb 3name. 1n4amen namen4s3 n3amer na3mn n2amo nams3 2n1amt namt2s n1an n3an.
    2nana 2nanb 2nand2 4nanf 2nan1g 2nank n3anl 1nann 2nanp n3anr 2nan1s 2nant n3anw
    2nanz nanz3l nanz1t nan2zw na1ot na2p 1napf n1ar n4ar. nar1a n3arc 2nard na1rei
    2n4ari na1rin n2ark nar3ma 2nart na3sit n1asp 1naß 2nat. 2nate nat2h 2natm nat2o
    2nats nat3st 1natu n1au nau1d 2nauf n3aug nau1ma 2nausb nau3sc naus2e 2nausg
    n2auso 2nausw na1w 2nay 1na2z nazi3t nä2 n4äc n1äf 4nähn 2näl 4n1äm 2n3än 2när
    när4s när1t n2äss 1näß 2n3b2 n5ba n7bart. n4be. nbe2in nbe1n nber2 nbi2 n5bod
    n4büre n1ca nc5ab n1cel n3chef n2chi nch1m nck2 n3cl n2col n3cr n1cu n2cy 4nd
    nd1ab nda4bl nd2a4g n1dah nd1ak nda3ma nd1ana nd1ang nd1ann nd1anz nd1arb nd3arr
    n1dars nd1art n1das nd1au nda1v nd1c nd3d n1deal nde3alt nde3g n1degu nd1ei
    nde3k nde4län ndel4sa nd3ents nde3o n2der nder3m nder5ste nde2s ndes1e nde3sig
    nde3v nd3g n1diä n3diens n2dier n1dif n1dik n1dio n1dit n1diz nd4lac nd1n n2dog
    ndo3ge n3dok n2dom n2don n2dop nd1ope nd1or n2dot ndo3z nd1rat nd1rau nd1räu
    n2d1re nd1rif nd1rob n2drog n2d1rö n3dru n2d1run nd2sor nd2spr nds3zi nd1tep
    nd1th nd1ti nd1t2r n1dua nd1uns ne2 4ne. ne3am ne3art ne3at ne3au neb2 ne3ba
    nebe2l ne3ber n1ebn ne3br 2nebu 3nece neck2a ne3ein ne3eis nee1r2 nee1t n1ef2
    ne3fa ne3fe ne3fl ne3fr neg2 ne3gä n1egg 3negr 3neh4 4neha 4nehä 4nehe ne3her
    4nehi 5nehm 4ne3ho ne3hu n1ei 2nei. nei2d 2neie nei2g 1neigt nein1d n3eing
    n3eink 2neis nel3b n1ele ne3leu ne3lie ne3lin 3nelk nel2l nel3lö ne3l2o 4nem4
    ne3ma n1emb ne3meng n1emi n2emo n3emp n1ems ne4n 2nen. nen3a n2enc n1endb n1endd
    nen3dep n1endf n1endg n1endh n1endk n1endp n1endt n1endw nen1eb nen3ga n2en3k
    1nenn n2ens nens4e nen1sk n1entb nen5tier n1entl n1entn n1ents n3entw nen3u
    n2enz nen1ze neo1 neo3b neo2p neo3r ne3pu 2ner ner3af ner4al ner3ap nerb2
    n3erbie n2erbr ner1de n1erf n5erfo n3erfü n1erg n1erh n3erhö ne3rit n1erlö
    n1ermi n1ernä n1ernt n1err n1ersa 3nerv. 3nervs 3nervt n1erz n2es 2nes. ne4sal
    ne3san ne3säng ne3s2c ne3serg ne3sh ne3ska nes1o nes1pa nes2s 2nessb n3essi
    ne3sta nes3tei nest2l 2neß3 net1ak ne3tal net1an n1etap n1etat net1au n1eth
    2neti ne3tor ne3tru net3sc 2nett n1etu 3netz 1neu neu3c neu1d neu3erf neu3erk
    neu3ers neu3erw neu1f neu1g neu1k 3neu1l n2eun 2neur2 neu1te 3neuv ne3vi ne3vo
    2n2ew n1ex 2ney 2nez ne4za ne3ze 2n1f nf1ak n3fal nfalt4 n3fä n3fel n4fer.
    n4fers n3fes nf1f n3fi nf2l nf3lan nf3läd nf3lin nf2o n4fora n3frag nfra1t
    nfrau4 nf3st nft2o nf2t1r nft4st nf1u nf2z 4ng n1ga4 ng2abs ng1ac ng1ad ng1ak
    n2gal ng1ala n2g1am n3gamb ng1and ng1anz n2g1äl ng1d n3geb n1gec n1gei ng1eif
    ng1ein ng3eise n1gele n4gelic nge4lis ngel1l nge4los n2gelt n1gem ng2en n1gena
    ngen3ze nge3rie ng3erse n1ges n3gesi ng2fe ngg3s ng2häu ng1id n1gin ng3land
    ng1län n2glic ng1lo n1g2loc ng1lö ng3m ng1ne ng3ni ng2no ng1ope ng1or n2gos
    n1got ng1rat ng1räu ng3rein ng1rev ng1roc ngs3au ng4scr ngs3eh ngs3pa ngs3pin
    ng4stä ng2to n2gue n2gum n1gy 2n1h n2hac n2ham. n3han n3hau n3he nho4 n2hud 2ni.
    nia4 nib2 nich2 nich3st nicht5er ni1cr 2nicu nid4 2nie. nie3b nie3c 3nied nie3fr
    niel2 nie3la nie4r 1nies 2nies. 4niestr 1nieß 2nif 4nig ni1g2a ni2ge n3igel
    ni2gl ni1go nig1r nig4sp ni2h 2nik ni4ke nik2er nik3ing ni3k2l nik1t n2il ni1la
    ni8laterali nil3d n2im nim2b ni1m2o n3imp 4nin ni3nac nin1al n2inb n1ind nin1de
    n2ing n1inh n1ink n1ins n1int n1inv nio4r ni1ra nir4w nis1e 1nish 2nisi nis1in
    nis2ku 1nisn nis1or nis1p nis3pe 1niss2 2nis2sa nis1sk nis1sz ni3sta nis1to
    nis1u 2ni2t n2ita ni3tag nit2r nit2t nitt5erk nitt4sa 2niu ni1v 1nixe n1j n2jas
    n1ka n2k1ad nk2al nk3alg nk3anz nk1apf n2kare nk1arm nk3art. n2k3aus n2k1äh
    n2k1äp nk1ec nke1d n1keil nk1ein n1kell nk3erfa nk4erg nker4l n1ket n1ki nk1inh
    nk1ins nkkom1 n1kl n3klet nk1lis nk3los. nk3m nk2na nk1nes n2k1nis n3kon nk1orc
    nk1ort n4köl nk1rät nk3rede nk1ro n2k1rö nk1sen nk1ser nks3ter nks2ti nk1s2z
    nkt3an nk2tau nkt3it nk1tor nk1tr nkt3ric nk2tru nk1tü n1ku n2k1um nk1uni nk1urt
    2n3l2 nlan2 nle4 n3m2 nmen2 n1na nnach4b nn1all nna3ma n2nan n2nau n1nä nnä3l
    nn1c nn1d nnea3 n1neb n2nec n4ne3g n1neig n1nek n2nel nne3mo nn4ens nnen3ta
    nn2er nne3ros nn3erwa nn3erz nnes1e n1ne4st n1net n1n2ex nn1g nn2hi nni2 n1nic
    nni3l n1nim nni4s n2niss n1niv nn1n n1no2 n2noc nn1ori n1nö nn2sa nn4sam nn1si
    nn3s2p nn4s3pe nns3tat nn2stö nn1ta nn1tel nn2th nnu4 nn1uf n1num n2nun nn1unf
    nn1ur n1nü nnvoll4 n1nym nn1z 4no. 2nob n1obh no2bo no1br n1ob2s no1bu no1cha
    2n1of no1fes nof3f no3g no1he n1ohr no3j 2no1k 4noke n1okk nok2l 4n2ol no1la
    nol3c no1lei no1li no2lin n3olm nol3sc 1nom2 2noma no1man 4nomet 2nomf 6nomial.
    2nomp 2no1mu no2my 2non 2nop n2o3pa no1ph n1opp n1ops no1q nor4 2nor. n2orak
    nord1r 2nori no3rin 1norm no1rö nor5st 2n1ort nos4 no3sa no3sc no1sh n2oste
    n1osth not1ei not3ent no3th not3in not1op 3nots 4nott2 2noty no2v nover4s no3vid
    no1wa n3ox 2noz no3zi 2nöd 2n1öf n1ök 2n1öl n2öt n3p4 n4plo npo2 n1q 4n1r4 nre1b
    n2regr nrei2 nre1sz n3rh nro2 n3rö nrös1 n3ru 2ns ns1ad nsa2f n3sai n2sang
    n3s2arg nsa1ri n3sau. n3saue n2saus n3säl n2s1än n2säus ns3be n6schobe nsch7werd
    n1se ns1eb nse4h nseh5ere nseh1r ns3einf n4sekti n2sel n4sel. n3selb n2sem
    n3semi n2s1ent nse2p n2serb ns1erf ns1erg ns1erk ns1erö ns1ers ns3ertr n3serv
    ns1erw ns1erz n2ses n3sess n2s1eu n4si. n2sib n4sier n3sig n1silb n1simu n1sin
    ns1ini nsinn4s n1sir n1skal ns3ke ns3m n3smara ns3n n1so n3soc nsof3f n3soh
    n3sol nson2s ns1op n3sopr nsor3b ns3ort. n3sos n3sou n3soz n2spat n4s3peri
    n4spers n2s1ph n2s1po ns3pot n2sprä n4s3prie n4spro ns3s2 nsse2 n1st n4stag
    nst1ak nst3ane n3star n3stat n4s3tat. n4s3tate nst3eif n6steil. n6steile n3stemm
    n5sterbe nst5erge n5sterne n5sterns n3s2teu ns3them n3stit nst5opfe n4strac
    n4strie nst2ü n2sty n1su n3suf ns2um n3sump n2s1un n2sur ns1urs n3sy ns3zie 2nt
    nt3abs nt2a1c n1ta4f nt4al n1tam n1tanz nt3anza n1tap nt2ar3b nt1ark nt2arm
    nta3rot nt2as nt4at n1tau n1tä nt1äm n2täte n2t1äu nte3au nt1eb n1tec nte1e
    n1tege n3tegre n3tegri nt1eh nte1he n1tei4 nt1ein n1tek nt2el ntel2a n1tele
    nte3lei nte3me n1temp n1tend nten3g n1tep nt4erh nter3m nt4ers nt4ert nt1ess
    nt3estr n1the nt1ho nt4hu n2thü nti1d n3tierh n2tig nti3kl nti1ma nt1inh n1tint
    n1tip n3tipp nti1r nti3sa nti1z nt3l nt4lit nt3m nt3ni ntons1 n2tos nto2t n3tou
    n1tö n1tr n2trab n3trak n3tran2 n4trane n3trä nt1rea nt1rec nt3reif nt2rep
    nt3rieg nt3rin n3trio n3trop n5tropi n3trö nt1rü nt3schr nt3sex nt1s2o nts2p
    nt4s3par nt3spo nts2ti nt2sto nt3str nt1su nt1t2 n1tu n2tul n1tü nty2 nt3z nu2
    nuar3 1nud 2nue2 2nu3f2 1nug n1uh 2nui 3nuk 1nul2 n2um. 2n3um3b 2numf 1numm
    2n1ums 2n3umz nung4 n3ungl nungs3 n1uni nun2m nu3po nur1 nu3re nur4m 2nus. 4nusg
    nu4s1i nu3skr nu3spo nus4su nus1t 3nuß 1nut nut3r nu3v 2nüb 1nüc nü3fu nü3ges
    nür3b nür3c nür3r 1nüs 1nüß 1nüt nü1te n3v n4vers. nvor3 2n3w n4wang n4war.
    nwei2 2ny. ny2m ny1r 2nz n2z1ag n2z1an n2z1ar n2z1au nz1än n2z1är nz3b n1zec
    nze4d nzel3a n1zell nzel3la nzel5leb nzel5lei nzen3so n1zent n1zep n1zerr
    nz5erste nz3erwe nzes2 nz3g n1zif nzig4s n2zing nz1ini nz1int n1ziv nz2le nz3n
    nz1of n2zop n2z1or nz3s nz1th nz1ti nz1wa n2z1wä n2zwir n2zwö n2z1wu nz1z 2o.
    o1a 2oa. o2ad oad4s o2ah oa3in oak1l oal2 o2ala oal3g o3an o4an. oan2n o2a4s
    1oase o3asi o1ä 2ob. o2bab obach2 o3bak ob2al ob2am ob2ar o3bas 2o3b2ä 2ob2b
    ob2e 2obe. obe1b 2obef o1befe ob3ein 2obel 2oben obend2 ober3in obe3se 2ob2i2
    ob3ite 3obj o1bl ob1lei 1o2b1li 2ob2lo ob2n 2obo obo3sk o3bö o1brü ob3s2h ob3sk
    ob2sta 2obu 2o3bü 2o3by o1ca o1ce och1a ochan1 och1ec och1ei och1l och3m och1n
    och1o och1ö och1r och3ri ocht2 och1w ock5ente ock2er ock3sa ock1sz o1cl ocon2
    o1cr od2a o1dar od2d ode2ga odel2s o4den oden1e oden3g ode2r o1derm ode3sp o1dia
    odia2l o2din 4odisti odium2 od2ka 4od4n o8dramatis o2dri odt2 od1tr 4o1du o2dun
    od2w 4o1dy o1e2 o5eb o2ec oen4d o2es o2et2 2ofa ofa3l ofa3m of4an of1au of2e
    o1fee of3ei of4en of2fa of1fi of4fil off1in of4fir 1of3fiz off1l of4fo off1r
    off1sh of4fü 2o1fi o2fib ofi1be o2fic o2fi1g ofi3k ofi3lig o2fi1m o2fis 2of1l
    of2lä of2lo 2ofo o1fr of1ra of1rä ofs1a of4sam ofsof3 of2spe of2spr of2s1u 2oft2
    of2te of1th o1f2u 2og2 og3ab3 og3ang og3aus og3b o3geb og3ei ogel1i ogen1t oger2
    o1gesa o2ge2t o3geti og3f og3ins og3li og3mu o1gn og3si og3sp og3ste og3to o2gu
    og3w o1hab o1hä oh1eis ohen2g oh1ert oh1erz o2hes o1hi 2ohl ohl1a ohle3b ohle3c
    ohl1ei ohl3erh ohle3se oh1lo ohls2e oh1ma oh2n ohn1ac oh3nat ohn1d ohn1o ohn1sk
    ohol3t oh1op o2h1ö ohr3a ohr2sc oh1s oh1ta oh2u oh1w 2o1hy oh3z 2oi o1im o1in
    oin1de oin3do oin1g o2ink oin2t ois2 oi1th oi4w o1j o2ja oja1m o3jet oka2 o2kar.
    o2kat ok3d oke2 o2kel o1kl ok1lau ok1lä o2kle ok2o2 o2kod oko3ri oko3t 2o3kr
    oks1p okt2 ok2z ol2 2ola ol3ak o3lal olar3m ol3auf 2o1lä ol3ät ol3ba ol3bi ol3br
    4old olde2 ol4dr o1leb ol3eie ol3eis oler2 ol3ex ol3fr ol4fra olge1g olg1r ol3h
    o1lif olik2 o1lim ol3ko olk1r oll1ak oll3ans oll1ec oll3ein oll1el oll5ends
    ollen3k oll3erk oll3erw ol4lo oll1ov oll3sp ol3ma ol3mi 2o1lo olo1k olo3pf o2lor
    ol3org ol3ort o2los o1lö ol3r ol5sä ol3schr ol1ser ol4sh ol3si ols2k ol3sp
    ol3tag ol3t2h ol3tr ol3tu ol3unt oly1ph olz3ern 2om o1mac o1mag o2man. om1art
    omaso3 oma1th om1au o2män o2mäu om2b om3bel om3bi om3bl om3br om4buc om1d om1ebe
    o1mec o2m1ei ome3la om1ene omen2t om1erh ome1ru om1erz om1ex omiet1 o3mil o1mim
    om1ind om1ing om1ins o2mis. o2mita omm2a om3meng om3ment om1mi om2n om3ne om3no
    om3nu o1mo o2mo3g o4mol om1org o2mö omp2 om1pe om1pf om1ph om2so om1st omt2
    om1th om2tu o1mus o1mut o1my onal1l on2ant on1ap ona2r on3arb onas3 onau1z o1nav
    on3äh on4bo on1ch on1do on2du on3duk ond4w o4ne3h o2nen onen3st on1erb on1erd
    on1erö onf2 on3fen on3fl on3fr on3ga on1ge ong2k ong2l ong2r ong3st on3gü o2ni
    onie3g oni1fe oni4k on3im on3ing onis4 onk2 3onke on3ki on1nu ono1c on3oke o3non
    on1orc ono1t2 on2rad ons1a on4sam onsen2 ons3h on1si on2sin ons1p on3sur ont2a
    on2tä ont3end ont3erw on2th on1tin on4trad ontro2 on3ums o2nus 1ony on1z onzer2
    on3zi oo1b oof2 oo2g oo1h oo2k ool3c oo2m oom3b oon2k o1op oo1pa o1or oor3da
    oor2g oo2s oos3s oo2t oot2l oo2v o1ö 2op. o2paa opa3b2 opa1d opa3de o2pa3ge
    o2pak op1akt o1pan o2pa1p opa1ra opa1re o1park opa3sc o2pau opa1v opa1ze 2open
    1opera o2pf 2opf. opf1a o3p2fä opf3erd opf1l opf3la op1flu op1flü 2oph2 o2pi
    opia2 o3pil o3pis. o3pit op1lag op1le op1li 2o1po op2pa op2pl 2oppt 2o1pr op3roh
    o2pros 1opsi op3so op3sz 1opt4 op1th 2opy 4or. or1a2 2ora. or3ab orab3b orad2
    or5adr or3alm or3ami o2ran or2and ora3sc or3att orauf4h or3aut 2oraw or1änd
    or1ät orbar4 or3bec or3bei or3bel or3bew orb2l or3bo 2or2ca or1cha orch2l or2d
    2orda ord1am orde2 or3dem ord3eng ord3ing ord1ir 1ordn 2ordr ord3st 2ordw 4ore
    o1rec or1eck o1red or1eff or3eig or3ein o1rek o2ren o3renn o1rep or1er or3erk
    ore2s ore3sc or1eth or1eu 2orf orfs2 2org. org4a or3geg 2orgen or3ges org2h
    2orgi or3gla or3gle or2gn 2or3gr 2orgs org2t org4w 2orh or5ho o2ri 3orient
    o3ries 4oril 4orin or1ins orin2t ork2a ork3ar or3kat or3ket ork4h or3kl or3kn
    ork2r 2orm orma4 orm3asp orm3ord 2orn or2nac or4n3ä or3niss or2n2o 2oro or3ob
    o2rol or1oly o1rom oro4t 2orö 2orp or2pe or4pu 2orq 2orr or3rh or3ri 2ors2 ors4a
    or3sh or3si ort1ak ort3an ort1au ort3ere ort3erf ort3erk ort3ev ort3ins ort3off
    ort1or or2tö ort3rau ort3räu ort1um 2oru o3r2uf or3un or3v or1ya or1z or3zu 2os.
    o2s2a os3ad osa3lin osa1mu osa1s o2s2c osch3ar osch3le os4do 2ose ose1g os1ei
    ose1in o1sex os2hi o1sho 2osi osi4a osin1g o1sit. o1s2k os3kr os3ku os2m os2n
    o3so4 o4sov os1pec o3spr os2s oss1ac oss3and os3sc os4se oss3enk oss3enz ossof3
    oss1or oss1p oss1ta oss3tr ost1a ost4art oster3e ost5erwe ost3ran ost1rä ost1re
    os1tri ost3roc ost3rot ost3uf os1um o3sü o3sy o1s2ze oß1el oß1enk oß1enz oß1ere
    oßof3 oß3r oß1te oß1th o1tap otar3t ot1au ot3aug ot1ä ot4bur ote4 o1tec o1teg
    ot1eib o1tek ot2em3 otemp2 o2ten o1terr ot1erw o1tes o1th ot1hel ot1hi ot1ho
    o2ti4 ot1im ot2in oto1b oto1c oto1l oto1ma ot1opf otor3k oto1se oto1t ot1ö o1tra
    ot1rad ot1re ot3rin ot1roc ot1rö ot1ru ot3sc ots2d ots1p ot4spa ot2spr ott1a
    ot2t3an otte4 ott3erk ott2ho ot1tra ott1re ot1url o1tü ot2wel otz2ki ot3zu o2u2
    ouf2 oug2 o3uh o3um 2oun o3unds oun2g oun2t ou3pa 2our our2g our3ga our2m ous2
    2ouv o1ü o1v o2v2a oven2s ove3s o3vio 2ovo o3vol o2vu 2ow o1war o1wat o1wä o1wec
    ow1ef ow3h o2win o3wo ow1st ow1t 2ox. ox2a ox2e ox1l ox3ti ox1tr 1oxy oy2 oy3st
    2o1z2 o2za o4zä o3zei ozes4 o2zi ozon1a o3zy ór2 órd2 ö2b öbe2 öbel3i öben1 öb2m
    öb2s öch2 öchs2 öchs5l ö4d öde2 öd3la öd2st 1ödu ö2f öf2f öf1l ö2g ögen2 ögens1
    ög1l öhl2 ö2k öko3 3öl. öl1a öl2b öl3ba öl3bi öl3bo öl3br öl1ei öl1em ölf1ei
    ölf3s öl1im öl1in ölk1le öl1lä ölle3m öl3m öl1o öl3pi öls2 öl3sa 2ölst öl1sz
    öl1ta öl1tep 1ölu ölz2w öm4 ön1d ön2e öne3b ön1g öni2 ön2n2 ön2s ön3sc önt2
    ön1ta öo1 öo2t öp4 öpf3l öpp4 ö2r2 ör3a4 örb3l ör3da örden1 ör3di örd4lin ör3ec
    ör3e2i ör3el ör3em ör3ere ör3erl ör4erz öre2s örf1le ör3fu ör3ga örge2l ör3gr
    ör3h ör3im ör3ku ör4le ör3ma ör3me örn2e ör3o ör3q ör3ro ör3sc ör4schi örs2e
    ör3si ör3s2k ör3sta ört2e ör3tei ör3tri ört2s ör3tu ör3une ör3unt ör3v ör3z ö2s
    2ös. ösch3ei ösch3m 2öse öse1g ös1ei öse3str öso3 ös2s öss1c ös2t öst1a öß2r ö2t
    öt3a öten3e öt1r öt2sc ött2w öt2z öwen3 özes4 4p. 4pa. 1paa 2pab pa3ba pa3be
    1pack p1adr paf2 pa1fe pa1fr 1pag2 pa1h 3pai 1pak pa3ka paki3 pa3kl pa3ko pa3ku
    pal2 1pala palat4 1palä 4pal3b 1pale 4pal3g pa1lig pal4m palm1o 4palt pa1m pam3s
    pand4 pan2da pan3de pank2 pan2n 4pannu pan2s2 pan3sl pant2 panz4 1pap4
    papier7end pa3pl papo2 2paq 3parad par3aff para3go par3akt par3b 4pard pa1rek
    2parer 2par3g par6gel. 3park. 3parke par4kr par3m par3n 1parol 2parp par3ra
    par3ru par3sc part2 1partn 3party 2parv 2parw par1z par3zi par3zw pas4 4pas.
    pa3sp pa1str pat2 2pat. pat3a 1patho 4pato 1patr 1pau2 p1auf pa3uni p3ausz pa1ve
    pa1w pa3zo 1päc 1päd päd1er 1pärc pät3eh pät3ent pät3h pät3s 2p3b 2p1c p5ch
    2p1d2 pde2 pe2 4pe. ped4a ped3l peed3 pe3f peg2 pei4 2peic peichel7t pe3im peit2
    2pej 2pek pe3kr pekt2s 2pel. pel1a pel3f pel3ink pel4ins pel3lau pel1lä pell4e
    pel3leb pel3lit 2peln pel1ta pel3v pe3mi 4pen. pen2d 4penden 2pene pen1k pen1o
    pens2 pen3sem 1pen2su pen1sz 2pent pen1te pen3tr pen1z 1pep pe4r 2per. per2a
    per3as per3au per1d 2pere 2perg per2ga 6periali 2perie 2periu per3nic 2per2r
    perr3an per4rä per2se per4so 1perü per3v per4zen pes2 2pes. pe3sp pess2 2pesv
    1pet petri3 2petu peu2 pe3v 2pez 4pf. pf1abe p2fad pf3ai p2f1ak pf1am pf1ans
    pf3are p2f1au 4pfe. pf1eim pf1ein pf1eis pfer2c pfer3m pf1f p1fie pf1ins p1fit
    pf2l pf1lam pf3lap pf3lä pf4leg pf3lei pf4ler pf1lin pf3lo pf3lö pf4lü pf3n
    pf3om pf1ra pf1re 1pfro 2pfs2 pf3se pf3sl pf1sz 2pft pf1th pf1ti pf1tu pf3z 4p1g
    p3gu 4ph. 4phab 3phag 1phas p1hau 2phär phe4 p1hei 2phel phen1d phen1s ph1ers
    p2h2i 4phi. 4phia 4phid phi4g phik3a 4phin phi4r 4phis ph1m ph1ni 1phob pho3m
    p3hop pho2t ph2r 2phro 4phs ph2t2 4phthe 4phu p1hü 1phy pia1b pia1m pia1r pia3s
    pia1t pi2c pi3chl 2pid2 2pi2e pie3fr piege2 pie4l pie3m pie4r pi4k piko4 1pil4
    3pilg pin2e pin2g pin3gen ping3s ping1t pin2s 3pinse pin3sk pin2z 2pio 1pip
    pipe3 pi2r pis2 2piso 2pit2 pite2 2piu pi2v 3pix piz1in 2p3k p4ke pkur3 1pl4
    2pl. pla4 p1lab plan1z plas4 plau4 3play 3plä 2ple. 2ple3c plen2 2pler 2ples
    pli4 2plif 2plig 2plis 4plosiv 5plosivi 5plosivk plut2 ply1 4p3m p2n p3na p3nä
    pneu2 p3nu p2o 2po. 2po1b po5ba 2poch po1da 1poe 2po1g 2poh po2i2 3poie 1poin
    1pok 1pol pol3g pol3li pol3sc pol4t pom2 po1man pon2 2pons poo2 po1ob 1po2p
    popa4 pop1ak pop1ar po3ph po3pt po1rau por3d po1rei porf2 2porn por3s por4t
    portbe5w 3porto. 3portos port3ri 1po2s4 2pos. 3posi 2po3sp post3ag post3ei
    po3str post3ra 1pote pot2h 2poti pot2t pot1u poxi3 2po3z pöst4l 2pp p1pa pp1ab
    p2pad p2pal p2pan p3par p2pea ppe3e ppe3g p2pen p3pendi ppen1t p2per pp1f pp3fe
    p2p1h ppin4 pp1lä p2ple pp1lei p1po pp3oh p4po1l pp1p2 p1pr pp3ra pp3rä pp3ren
    pp3sa pp3se pp3si ppt2 pp1ta pp1tel pp1ti p2py1 p2r2 prach7tes 1prak 1prax 1präd
    1präl 3präm 3prä3s prä3t pre2 4pre. 4prec 1pred pree1 2p3reg 1prei4 3preis
    preis3c 2preiz prem4 pren2 pres4 8pressionisti pre3v pri4 2pric prin2 3prinz
    prit3e 1pro pro4be pro2bi 2proc 3pro1d 3prog 3proj pro1m pro1p 2pross pro1st
    2proß 2prö prun2 1prüf 2prüh 2prün p2s 2ps. p4sal ps4an p3sat p3sä p3sc 2pse
    p3see p3s2h 2psi ps1id ps2l ps1ö p3sp 2pst p3sta ps2te p3stea p3stel p3sti p3str
    ps1tur p3stü ps2w 3psy 4psys ps2ze 2pt pt1a pt2ab pt3at p1tä pte4 pt3ec pt1ei
    ptem4 pt1ent pt1ep pt3erei pt1erw pt1erz pt1h pt1in p4tor pt1r pt3ri pt1s2o
    pt3sp pt3st pt1su pt1t pt1um pt1urs pt3w 3p2ty pt3z pu4 1pub2 2puc 3pud 1puf2
    p1uh pul2 2pule pul3in pul4s 1pul4v 1pum pum2p pump1l 1pun2 2pund 1pup 2pur
    pur3r put4e 3putz 1püf pül1l pür3g p3v 2p3w py3d py1e2 1pyl py1s p1z p2zi pzu2
    p3zy 2qi qi1g q2u2 quan2 2que. quen2 2quiem qum2 4r. 4ra. 2r1aa ra1ar4 r1ab
    ra3bal rab2b r2ab2er ra3bew 2r4abi ra3bit r3abw ra2c r3acet rach3b racht3r r2ack
    r2ad rade2a 3radel 1radf rad3t4 2rae r2af raf1ar raf1er raf1r rafts2 rage3ru
    ra4gi ra3gle ra3gr ra2h ra3her r2ahm 2ra3ho 2raht rain2h r3air 2rak. r4ake
    ra3kin ra1kla 2rako ra3kos ra3kot ra3kr 2rakt 2r2al ral3ab r3alar ral1de ra1lex
    r4ali ra1lib ra1lip r3alk. rall2e ral3lin ral1lo r3alm. ra1log r3alp. ral1sk
    r3alt ral1th ra1ly 2rama ra1man ra1mar 2rame ra1mee r2ami ra1mil ramm5ers
    ram3ste 2r1amt ramt2s ra1mu 2ran. 2rana r2anbe 2ranc ran2d r4anda r4ande rand3er
    ran3dra 2rane r3anei r4aner ra3net 2r1anf 2rangr 2ranh 2rani r3anil r3anl r3anm
    r4anmu ra3no r1anp ran2q 4rans r2ansp ran4spa 2rant ran1ta ra3nü 2r3anw ran2zw
    r2ap ra3per ra3pl ra1po r1aq 2r1ar r3arb rar3d ra1rei rarf2 rar1in ra1ro rar3r
    rar3u r3arz r2as 2r4as. ras4a ra3schw 2rasd ras2e ra3sed ras4mu ra3spr rass2s
    ra3sy 2raß raß2s 2rata r4atl rat3m r4at2o rat2r rat3sc rat3ste 2ratum 2ratz
    2rau. 1raub. 2raud rau4e 2raufb 2rauff 2raufl raug2 rau2m raum3ag rau3mel 1raup
    2raur 2rausb 2rausg raut3s ra2vo 2raw ra3wo r2ax raxis1 r3axt r2äd rä1di 2räf
    rä1fe rä1fi rä1fr 2räg rä1hi rä3ka rä1li 4räm rä1mis rä1mo 2ränk 2räq 2r1är4
    rä1ra rä1ro r3ärz rä1se räte3te 1rätse 2räue räus2 2räut 2r1b r3ba rb1ab r4bacht
    r4b1ade r4bals rbar4g r4bari r5barin rb1art r5bas rb1auf r3bä rb3b rb1ech r3beck
    r2bee r2befa r3beh rbeid2 r2beka r3bela r2belä r3benu r3benü rbe4ran rber2e
    rber4ge r4bergs rbes2 r2besa rbe3se r2bet. r2beta rbe3to r2beur rbi4 r3bil
    r2binn r3bio rbit2 rb2la rb3lac r2b3lan r8b7lasser r4b3last r3blat r3blä r3blec
    rb3leid rb3ler r2bleu r3blö rb2o r2bol r3bot r3bö r3b2r rb3rad rb3res rb2s
    rb3ses rbs1o rb4stä rb3stre r3bud r2bunt r3bus r3bü r3byt 2rc r1ca r2ce rce1r
    rch1au rch1ei r3chem r2chi rch1l rch3lö rch3m rch1r rch1s2 rch3sp rch1t4a rch1w
    rck1 rck2sc r1cl r3co r1cr r2cy 2rd r1da r3d2ac rd5achse rd1ak rd1al r2dame
    rd1ann rd1ant rd3anz rd1arb r3dat rd3d r1deb r3dec rd1ei rd2ei. rd1elb rd1ele
    rdels2 rde1mi rde3nar r1denk r1dep rd3ernt rdes2 r3desi rde3sp rd1eur r2dex
    r1dez rdgas3 r3dif r2dig r3dik rd1inn rdio8logische rd1ita rd3l rd2lo rd4loc
    rd2n rd3na r1do2 r3dor rd1os r3dow rd1ös r3dr rd1rat r2d1rau rdre2 r2drin rd2rü
    r2ds rd3s2k rd3sz rd1t2 rd2tis r1du r4dunge rd2wa r1dy 4re. re2am reas3 re2b1a
    2rebi re2b1l reb1ra re4bu re2bü r2ech rech3ar 3rechn 2reck. reck1l re4cl 1reda
    2redd 1rede. 2redi red2m re2dun 4ree. 1reed ree4m re1eng re1er ref2 re3fa 1re3fe
    3refer 2reff 2refi 3re3fl 3refo 1reg re2ga 4regef re2ge2l regel3ä rege2n re3gene
    r1eg2g 3regi re2gl reg2n re2go re3gr 2reh reh3ent re2h1i reh1l4 re2h1o 4r2ei.
    2r2eie rei1gl reil2 r1ein rei3nac rein4al re1ind 2reing rein8s7tre re1inv rei1r
    rei3sc reis3m reis5tro rei2ta rei3tal rei3the reits2 2reiv 2reiw re2ke 2reki
    1rekl re1ku rel2 4rel. r2e3la re1leg 3re3lev re1lik re1lis re3lu 2rem r2emi
    re1min re2mo remp4l rem3si re2mu 4ren. 1re3nai re3nat ren2d ren3dam ren3die
    3rendit ren3dr re1neg ren3end ren4ers 2reng 2re2ni 2renko r2enn 4renna re2nom
    ren3sau ren3tal ren3tel r1entg r3enthä ren7tier. ren3tis r1entl ren3tr r1ents
    2r2enz renz5erf re2ob reo1p 1repe re2per 2re2pi 1re3po 2repp 1re2pu re3put 2req
    4rer r1erb r2erbr r1erd r3ereig r1erf r1erg r1erh re1rie r1erk r3erken r1erl
    r4erlag rer6lei. r1erm r1ernä r3erns r1ernt r2ero r1erö r1ersa r2erse r1ert
    r2erte r3erz 4res. re2sa 2re2sc 1resol re3son res2s ress2e ress5erw res4so r2est
    re4stec re1str re2sum 2resz 2reß re1tai re1tal re2tr re3tre 1rettu re3tus re2un
    re3uni r1eur reut4l re3vis re3vo 2r1e2w r3ex1 1rez 2rezi 4ré 2r1f r3fa r4fahra
    r3fä r2feit r3feu rf1fe r3fie rfin4 r2flan r3flä rf4ler. r3flü rfolg4s r3fö r3fr
    rf2s1ä rfs1id rf2ta rf3tau rft2r rf1tu r2furt r5fü 2r1g rga4 rg2ab rg1ad r3gag
    rg1ah rg1ak rg2an r3gang r4gans r3gas rgas4m rgas4t r3gau r3gä rg1d rge4an
    rge2bl r3gef r3geld r4ge4l3er rgel1l r4genen rgen4z3w rg2er r2gern rg3e4tap
    r2ge2ti rg1eul r3gew rg3g r2gi r3gin r3giss r3git rg1lo rg2log rg3m r2g1na rg1ne
    r2g1no rg1ob r4gon rg3op rg1or rg1öd rg3ral rg1res rg1ret rg1rin r3gro rg1rüs
    rg3s2e rgs2t rg1sti rg3str rg1stu rg3su rg1th r2gu r4gun r3gü rg3w r1h2 2r3ha
    rhan2 rhand4l r4hard. 2r3hä rhe4 rho4 5rhoe r3hö 5rhö. r3hu r3hü 1rhy ri2 2ria
    ria2l rian2 ri3ar ria3tro ri4b2 1ribs 1ric 4rich. 4riche 4richs rich2t 2rick
    4rid rid2g 4rie. rie3ber rie3h rie3la 4rien 2rier rier1d rie3ro rif1ei rif1er
    ri4f1o rif1r rif3st 2rift 2rig 4rig. 4rige rig2g ri3gr ri3he 4rik ri3kol ri3kop
    rik1to ri3la ril3lin ri3lo r4imb ri3mes ri3met rimm2 rim2s 2rin. 2rin2a r3indu
    2rin2e rine3i r1inf rin2g ring1l r1inh ri3nitä 4rin2k r5innenm r3inner r1innu
    rins2 r4ins. r4inspi rint1r r1inv 2rio ri3pa ri3pe r1ir ris2 ris4a ris4b 2risc
    3risik 3risk ri3sko ris4m ris3p 3riss rist5ers 1riß riß1e riß1te r2it2 2rita
    r3ital rit3ant riten3 rits2 rit3sp rit3w ri3wi rix1 2riz 2r1j 2r1k r3kab rk1all
    r3k2am r3kas rkauf2 rk1aus rk1äh r3käu r3kee rk1ef rk1ein rken3te r2ker r3kla
    rk2las rk2lau rk1lis rk2ni r3ko rko4g r4kot r3kr rk1räu r4k1rea rk3rin rk2s1e
    rk2sp rk4stec rkt3eng rkt3erf rkt3ers rkt3erw rkt3erz rkt1o rkt3r rk1tra rk2tre
    rk2um rk1uni r3kup r3kur r2kus rkus1t r3kü 2r1l r5lad r3lamp r3lan r6langs.
    r4langv rl1ar r2l1asc r3lau2 r4l3aug r5lä rlber2 rl2bo rl1c r3le4 r4lec r2len
    r4leng rle5th rl3f rl3h r3l2i r4lind r4lins r4lith r2litz rlk2 r3l2o2 r4lon
    r4loo r5lor r4lot. rlös3s rl2s1p rl2sto rl1ta rl1tep rl1th rl2to r3lu r3lü r3lym
    rlz2 2rm rm2abe r3mac rm1ad r2maf rma3ges r1mang rm1ank r5mann. rm1ans rm3anz
    rm3aph rma1ri r3maß rm3d2 rm1ef rme3le r2meli r2mem r2mer. r1merk rm1erp rmet3t
    rm4ga rm4hi r3mik r3mil rm1im3 r2min r3minz r2mis. rm3lu rm1m rm3n r3mob r2mom
    rm1ope rm1ori rmo3sc r3mot r3mö rm1p rm3sa rm3sta rmt2a rm1ti r1mu rm1ums r3mus
    rmvoll3 4rn r1na r2nan rn2and rn3ani r3nann rn3anz rn3are rn5ari r4naß r2nau
    r3nä rn1ähn rn3c rnd2 rn1da rn1di rn1do rn3dr rn1du rn3eben r4neg rn2ei r1neid
    rn3eif r1neig rn3eis rn1ene rn1erk rn1ert r3nerv r2nes r3neste r1n2et rneu4e
    rne3uf rn3g2 rn2har r1nim rn1in r1nip r1nis r2nisa r2nism r2nist r3nitr r3niv
    rn1n rn1op rn1or r3not r1nov rn1ö r1nöt rn3sa rn3s2ä rn4sche rnse2 rn1si rn3s2p
    rn1s2z rnt2 rn1ta rn2ten rn1th rn1ti rn1to r1num r3numm rnun2 rn1ur r1nü r3ny
    rn1z 2ro. ro3ai ro2ban ro2beh ro2bew ro1bri roch2 ro1chi 2rockn r4ode 2roe ro3et
    2rof ro1fe ro1fl 2rog. 1ro2ga 2rogs roh1l 1r2ohr ro3ir 2r2ok2 ro1ka ro1ki ro1lab
    ro1lag 2rold role4 ro1lei ro1lep ro1lig roll4en ro3lo 2rols rol3st ro1ly rom3ber
    rome4 rom1el romen2 ro1mi 2romm ro3mö rom2p r2on ron2a ro1nau ro2ne 1ronn rons2
    2ron2t ront1u ro1nu ro1of 1room ro1os 4rop ro1pho 2r1or r4ora ror3al ro1rau
    ror2g ror1m ror1o ro1ru ros4 ro3san rosen1 ro3sk ross1c ro3st2a ro2ste ro2ta
    rot3sa rot1s2o ro1t2u 1roul ro3unt 3rout ro5vers. ro1wa ro1we ro1wi ro3za röd2
    2r3öf 1röhr r1ök r1ölp 1römi rön1c r1ör rö3si 1rötu 2r1p4 r3pa2 rper3in r3pf4
    r2ph r3pl r3po4 r4pon r4poo rporte4 r3pr r2ps r3pu r5pum 2r1q r3quo 2r1r r4rac
    r3rag r2rai rran2 rras2 r2rass r3rau r3rä rr1äm rrb2 rr1c rr1d rr2e r3rea r3rec
    r3red r2ree r3reg r3rei r3re1l r2ren r3renn r3rep rre3pe rrer4s r3rese rre2st
    r4restr rre2t r3rev rr3g r4rha rr2hos r2rhö rrie4 rrieg7ler r3ries r3riß r2rit
    rr2j rr3m rr2n3a rr1ni rr2o rr3ob r4rog rror1t rro2t rr2sa rr4sche rr2st rr1tu
    r3ruh r3rum r3rup r3r2ü rr3v r4ry rr3z 2r1s rs3ab r3sabo rs1ad r3sagen r2salb
    r2sald r4s1amp r4s1amt rs2an r2s3ang rs3anp rs3ar r5sax r3sä r4säm rsch4a
    r3schat r3schü rs1ebe rse2e rs1ef rs1ein r3sek r2sell rs2end2 r2sep rse4ph
    rs1ere rs1erö rs1ers rs1erz rs1eta r2sh r3s2hav r3shir r3sho r2sib r7sierteste
    r5sierth rs3ke rs2kl r2skr r2sku rs3l rs3mu r2son rson4e r2s1op rsor4g rs3ort.
    r2sos r3sp rs3pers r4s1ph rs2pl rs3put rs3r rs1s2 rsse2 rs3st rs3tale r4stant
    r4steil r6st5eing r4sten r3sterb rst3erw r4stet. r3steu rst1h r3stie r2stin
    rst3ing r3stink r2stip r3sto r4s1tot r3stö r3s2tr rst3ran r6strang rs3tren
    r4strun r3stu r3stü rsuch2 r2sum. r2sumf rs1un r4sus r3swi r3sy 4rt r3taf rtag4
    rt3akr rt1alp rtals1 r2t1am rt2ame rt2anb rt1ang rt1ann rt1ant r1tap rt1ar
    rt3are r1tas r3tast rt3att r1tau rt1auf r1tä rt1ärm rt1ärz rtbe4w r3tec rte1d
    r1tee. r1tees rte3g rt1ein rtei3na rtei1s rtel2a rte3li rtel2l rtel3li rtem2
    r1temp r2te4n rten3so rten3st r1tepp rte1ra rt3erei rt1erh rt3erla r1term
    rt3ernä r3terr rt1ers rt4ersp rt1erz r2tes rte1sk rt3euro rtge3h rt1he r4thei
    rt1hi r1thu rti4 r1tic rt1id r3tief r2tim rt1ima r2tin rt1int rt4is rt4lig rt2nä
    r1toc r2toi r1tol rto3p r3tor. rt1orc rt1ort rto1ta r3tou r1tö r1tr r4t1rak
    rt1rec rt1rek rt1res rt3ris rt1ros rt2ruh rt5sac rt2s1eh rts1ei rt3sex rt2spa
    rt2spu rt1t rt3tr r3tuc r1tum rt2una rt1up r1tur rt1urt r3tut r1tü rt3w rt3z
    rtz4a ru2 ruar3 rub2 rude2 1ru4f ruf1f ruf2s 1ruh ruh4m 3ruin r3umf ru4n r1una
    run4d rund1a r2unde rund3er 2rundi r1unf run2h r1uni r3unio run2k r1unl r1unm
    2run2n r1unse 2runt run1zi ru3pl ru3po r1ur1 rur2g rus2l ru3s4p rus2s rus3sc
    rus4se russ1p 2ruth rut3he rut1o rut1r 2ruz2 2rüb rüh1la rüh1n 2rümm rün3f
    rün3sc rün3z rüt4 2r1v r2van r2vat r2veg rvel2 r3velo r2ven rven1e r2vern
    r8versität. r2ves r4vet r2vi r4vig r3vill r3v2o r4voi rvo3le r4vu 2r3w rwei4
    r4wem r4wins rwo2 rx4s 2ry ry2b ry1m ry2me rys2 2rz rz1ar rzäh2 rz2bo r1ze
    rz1eck r3zei2 r4zele r2zen rz1eng r3zent r4z3ents r2z1erf rz1erg rz1erk r2zerl
    rz1erq r3zers rz1erw r2zes rz1id r1zie r1zif r3ziga rzi3l r3zim r2zing r3zins
    r1zit r3zof r3zon rz1or r1zö rzt1ro rzug4u r3zuk rz1urs rz1wa r3zwä r3z2wec r3zy
    4s. 1sa 4sa. 3s2aa 2s1ab sa3ba sab4bat sa4be sach3th 2sad sad2d s1adm s1adr
    2s1aff sa3gan sa3ges s1agr sa2h sai4 4sai. 2s1ak2 4sakt 2sal. sal2a s1alar 3salä
    sal2b sal3bl sal2d 2sale 2salg 2s2ali sal1id 2salk 4s1all 2salm s2alo 3salon
    2s2alp 2salr 2sals 2s1alt sal1th sal2v 2salw 3salz. 3sam 4sama 4samb s2ame
    s3ameri s1amma sammen3 4s1amn sam4t s1an 2san. 2s3ana 2s3anb 2sanc s2and sand3ri
    san1er 3sang. san1ge s3anh 2s3anl 2s3ans sans2k 2sant 2s3anw 2s3anz 2s1ap s2aph
    sap1p sa1pr 2s1aq 2s1ar4 s4ar. sara4 s3arb 3s2ark s3arm sa1ro s3arr s2ars sar5ta
    sa2r5u 2sas. s1asi s1asp 6sast sa3stu sat2 2sat. sat3ant 2sate. 2saten 2sates
    2sath 2sati 2s3atl 4sato 3sat4z satz3en s1a4u 3sauc 2s3aufb sau2g sau2l 3saum
    sau1ma 3sauri 4saus. 2s3ausb sau3sc 2sauß sa2v 2savi sa3vie sa1w 2say 1sä 3säc
    3s2äg s1äh s3ähn 4s1ält 2s1äm sä1ma 4s1änd s3äp 2säq 2s1är 3s2ät 3säu 4säuss
    4säuß 4s1b2 s3ba sbe1 sbe2i sber2 sbil4 s3bl s3bü 1sc 2sc. 4scap 4scar 4s1ce
    4sch. 3schaf s2chal 4schanc sch3ans 4schao s3chara sch3arm s2chau 3schä 2schb
    4schc sch2e 4sche. 2schef sch3ei. 3scheib 4schem. 4schen. 4schend 4schens
    4scher. scher3c sch5erla 4sches 2schet 2schh 4schig 3schim 4schiru 4schk s2chl
    sch4lag sch3nis 4schonn 3schop sch3rom 4schron 4schs2 sch3sa sch1se sch1sk 4scht
    sch1t2a scht2r 3schul sch3z 4s3cl 2sco 3scop s2cor 3scr s3cu 2s1d2 sde2 s3def
    sde4k sde3s sde4si s3di sdien4e sdi3st sdi3v s3do2 4se. 2sea se3an 4seb2 s1eben
    se1ber se1ca sech2 2s1echo 2s1echt se1di 3seea see3ig see3lin see2lo seen2e
    see3r2e se1erk se1erö see1t 4sef s1eff se3f2l s2eg2 se1gem se1ges se2gl 1segm
    1segn 1seh seh1a se1hef seh1ei seh3erk seh1l seh3ner seh1ra seh1ri seh1s seh1te
    s1ei. sei2de 2seie 1seif 2s1eig 1seih 3s4ein. 4s1einb 2seind sein2e 2seinf
    2seing 2s1einh 2seini 2s1eink 2seinl s1einn 2s1einr 4seinsp 4s3einst s1eint
    2s1einw seis2 3s2eit seit2s 1s2ek 4s3e2ke 4seko sekom1 3se2kr se1kul se2kün
    sel1a se2lau se3lä 2sel1d2 se1leb sel1ec se1lei sel3ers sel3f sel4he 4s1elix
    2selk sel1la sel3leb sel3lif 4selm se3lo se2l1ö sel1sz 2selt sel1ta sel1tr se2lü
    1sely sem4 2sem. 1semin 3semm 2s1emp se2n 4sen. sen3ac 2senb 2send. sen1da
    5sendend 5sendens 5sendest 3sendet s1endl sen3do sen3d2r 2sene se3net 2seni
    sen3kl 2senl 2senm se3not 2senr senst2 sen3tal sen3tig sen3tis sen3tr s1ents
    sen3u 2senw sen3zin 2sepo s1epos 4ser. ser3ad ser3al serb2 ser2bi 2serc 2sere
    4sere. se1rea se3reic s3ereig ser3eim 4serem ser3enk 4ser2er 4seres se3rest
    2serf s1erfo s2erfr s3erfü 2serg s1ergä s4ergr 4s1erh se2ri 2serin 2serk2
    s3erken s1erkl 2serl ser5li 4serm 4sern s1ernä s3ernt 2sero se1rol 2serö s3eröf
    2serp 2serr 4sers sers2t 2serta 2serte 4serth ser3the 1serts 2sertu 4sertw s2eru
    se2ruh ser2um se3rund 2servo 2serw 2serz 2ses. se2sel se1sk sess4m ses3z 1seß
    1set 4seta se3tab s1e2tap 4setä 4sete se1ter 4s1eth 4seti se1to 4setr set3sc
    3setz 3seuc se1un 2seus s1exe 2sexp 2sey2 2sez 4s3f4 sfall5er sfi4 4s3g2 sga6
    sge1 s5gem sge2o sger2 sges2 s1h 1s2hak 2shal 2shan 4shä sh2e4 sher2b s2heri
    sher2r sher4sc sher2z s2him sh1ma sh2n 4shoc 1shoe 4shof 1shop 3show 4shö sh2r
    sh1te 2shu 2shy si2 2si. 4sia 1sic side2 s1ideo 2siduu 3sied sie3f 3sieg siege2
    siege4s 1sieh 2siel 4sien 2sier sie4s 3sieu 4sif2 2sig. sig1a 2sige sig2l 3signi
    2sigs sig4st 4sik sik1ab sik1ä sik1el siko3 siks2 sik1t2 sil2 2sili 2sils sil3sc
    si3me 2s1imm simme1 sim2s sin4a s1ind 2sine s1inf 4sing. sin2g1a sin2gr sing3sa
    2s1inh 2sini 3sinnl s1inq 2s1ins 2s1int sin3teg 2s1inv 4si4o sion4 sio2p si3pe
    si3rin 3siru 4sisc sis1e sis1p sis3z s2it 2sitä 1sitt 1situ 1sit2z 2siu si3un
    2siv 2siz 4s1j s1k 4sk. sk2a 2skab 2s3kam 3skanda s3kann 4skanz 4skap 2skas
    ska5str 4skateg 4s3kä 2skb s2ke s3kenn 1skep 2sker 1ski. s2kif ski1g 2skir ski3s
    3skiz sk2l 2sklas 1s2klav 2s3kn s2kog 2skol s3kom sko2p 3skop. 1skorp sko2t 4skö
    sk1q s3kra skre2 2skro 2sks 2sk1t s3kub s3kug 3skulp s3kun s3kup 2s3kü 2s1l2
    4sl. 3slal s3lan s3lä sl3b sle3bes sle4g s3lei sliga1 s2li4m 3s2lip sli3r 4sln
    slo2 s3lus s3lü 2s1m2 s3mac sma3d sma1la s3mä s3mee s3mei s3meng smen2s smer4
    s3mit s2mu s3must s3mut s3mü 2s1n2 4sna sna3b2 s2nen s2nos 4snot 4snö s3nu s3nü
    s2ny so2 2so. 3soa 2s1ob so3ba s2oc 3sod so3et so3fer 3soft 3sog so3ga so4gen.
    so4ges so3gl s1ohe so3hi 1sohl 2s3ohng 2s1ohr 3soj so4k s2ol so3lan 3sold sol3ei
    sol3s 2s3oly som4 so3me 3somi so3mit so3mo s2on 2sonad 2sone son3end 3song 3sonn
    3sono son3sä son2s1o son1t2 3so3o s1opf so3po so3pro sor2 2sor. sor3a s1orc
    2s1ord 2sore s1orga s3orgi so3rh 2sori s1orie 2sork sor3m 2sorr 2sors sor3sc
    sor3st 2sort. sor3u 3so3sc so3se 3soss s1ost s1osz 3soß s2ot so3th so3to so3tr
    so3unt sou3te 3sov s1ove so3vi 3sow so3wa so3we so3wi 2s1ox 1sö s1öf 2s1ök 2sön1
    s1ös 1sp4 2sp. 4spaa 2spak 2spala 5spalt s2pan 3spannu s3pano s3panz 4spap
    4spara 2sparl 2sparo 5s6parten 3s2paß 2spau s1pav s2paz 3späh 2spär 2spe. 3spei
    8spektivi 4s3pensi s2pera 4sperat 3sperg sper2m 2spero sper4r 2s1pers 4spet
    3spez 4s3pf 2spha 3sphär 3s2pi s4pie 4spier 4s3pil s4pinn 4s3pip 4s3pis s1pist
    6spizien 2spl 4s3pla 4splä 3s2pli 4s3plu s1pn spo2 4s3pod 2s3pog 3spoh 4s3pok
    4s3pol 2spop 4s3pos 4spote s4pott 4spr. s2prac s2pran 2sprax 4spräm 4spräs
    3sprec 4spred s2pren 4spres 3spring 4sprinz s2prit 4sprob 4sprod 4sprog 4sproj
    2sprop 3spross 4sprot 2sprüf 4s1ps 2spt 3spuk 3spul 2spup 3spur 4s1put 4s3py
    2s3q 4s1r4 s3rak sre2b sre1l sre1ta sri4 sro2 srös1c s3rü 4ss ssa5ber ss1aj
    ss1alb ss3amt ss3ang ss2ant ss3att s4sau. ss2ä s4sco sse1b ss1ec sse1ec ss3ega
    sseh2a sse1hu ss1ein ss4eind sse3inf sse3int ss1eis sse2ku ss3elek sse3n4ac
    sse5rat. sse5rate sse5rats sser5att sser1d ss1erö ss3erse sser3ti ss3estr sse1ta
    sse3v s1sieu s3skala ss3ke ss2lig ss3m ss1off sso3m ss1op ss1ori s4sow s2söl
    s3s2pe ss1pis s2spro ss1s4 ss3sa ss3sp ss3st s1st sst2a s5stad ss3tele s3stern
    s2stet s3stil ss1tis ss1ton ss1tor s3strö ss2ur 4st. 1sta 4sta. 3staa 2stabb
    3stabi st2ac 3stad 3staff 2stag 3stah 2stak s1tal. 2stale 2stalk st1alp st1alr
    3stan2d 2stann 6stantie 2stanw st3anza sta2p 2stapo 3star. 4s1tari s2tars 2stas
    2stat. 2st1auf 2staum 3s4taur 2staus 3staus. sta2v st1ave 2stax 3stä 4stäg
    4stält 4stänz 4st1äp 5stär s1täus 2stb 2stc st3ch 2std ste2 4ste. 2steam ste3b
    4stechn 1steck 4s1tee 3steg steg1r 3steh 1stei4 st1eid 3steig ste3k 1s2t2el
    4stel. st3elbi s3telem ste3li stel2l stell3ä 4stels 2stem 4stem. 2sten ste4na
    st3ends sten3tr steo1 2ster 6ster. st5erbie st3erfü st2erg ster6loh 4sterm
    3sternc 4stes stes3ta 1steu 3steue st3eun st1eur st1ev ste4w 4stex st3exa 2stf
    2stg 4sth s2t3ha st1her st1hi st3ho sti4 2stia 2stib 3s2tic 2stie. 3stiel 2stien
    3stif 2stig 2stik 3stim s2tin st1inb st1ins st1int 2stio 1stir 2stis 1stitu
    2stiv 2stj 2stk stkom1 2stl st3la stle4 st3lu 4stm 2stn st3ne 1sto 2sto. s1tob
    st1obs 2s1tod 4stod. s2tode 3stof 6stoffir sto1m2 sto2ma st5omn 2ston 4ston.
    4s1too sto2p st1ope 2stopo 2stor. 2store 2storg 2stori 2stors 2stort 3stoß
    4s3tou 4sto1w 4stoz 1stö 2stöch s1töl s2tör 3stöß 2stöt 2stp 2stq stra2 2strad
    3s2traf 2strag 3strah 4strahi 4strai 4strak 4stral stran2 4strans 1strap 3s2tras
    5straß 4s3traum s2träf 4sträg 1s2trän 4sträne 3streb 1strec 4stref 4s3treib
    3st4reif 2strep s2tria 2s1trib 4strig 3s2trik 2stris s2trof st3roll 3strom.
    2stron 4ströp 1stru s2trum 2strun 2strup 2sts2 st3sa st3sä st3se st1s4k st3sl
    st1so st3sp st3st st1su st1sz 2st1t2 st2u 5s2tub 2stuc 3stud 4s1tue 3stuf 3stuh
    stum2s 4stun. 3stund st3uni 4stunn 4stuns 4stunt 2st3url stur2m 2s1turn 4st3urt
    2stus 1stut 1stü 2stüch s2tück 3stüh 2stür. s1türe 2stv 2st3w 3styl 2st3z su2
    3sub3 4subi 5subv 3suc 1sud s1uf 1sug sugge4 2s1uh suhr4 3suk sul2 sult2 sum1el
    sum5ents s3umfa 2s3umfe 3sum2m sum1or s2ump 2sums s3umsa s3umst sum1t s1una
    sund5erh sun3dr s1unf s5ungena s3ungl s1uni 2sunt sun3ten 3s2up sur2d sur2f
    2s1url 4s1ur1t 2s4us1 sus2s 1sü 4süb süd3 2süme 3sün 3süs 3süß 4s3v 2s1w swei4
    4swie 4swil s3wo 1s2y4 4sy. 4syf sym3 syn3 3synd 4syp 3sys 4sys. s1z2 2s3za
    2s3zä 2s3zei szen2 1szena 1szene 2szent s2zes s2zeß szi4 2szie s2zin s3zins
    s3zir 2s3zu 2s3zü 2s3zw s3zy š2k ß3a ßal2 ßan1 ßar4 ß1ä ß1b ß3ba ßbe1 ßbe2i
    ßbe2to ß1c ß1d ßde2g ßdi3s ße2 ß1ec ß1eg ß1ei ßel1a ß1elek ßen3a ßen1da ßen1sz
    ßen1te ßen3tr ßer1d ß1erf ß1erö ß1erw ß1estr ß1ex ß1f ßfi4 ß3g2 ßge2bl ßger2 ß1h
    ßher2 ßig1a ßig4s ß1in ß1j ß1k ß3kl ß4kopfs ß1l ß2lig ß3m ß1n2 ß2ne ß3nef ß1o
    ßor2 ß2ort ß1ö ß1p4 ß3pul ß1q ß1r2 ß2ri ß1s2 ß3sa ß4schnü ß3schu ßse2 ßser2
    ß3st2 ß1ta ß1tä ß1tec ß1tei ß1tel ß2tem ß2ten ß2tes ßt1h ßti2 ß1to ß1tö ß3tr
    ß1tu ß1uf ß1uh ß1um1 ß1uni ß1ü ß1v ß1w ß1z ßzu2g 4t. 2ta. tab1an t1abb 2tabf
    2tabi 2t1abk t3abn ta3bo t3abt ta3by 2t1ac ta2ca tad2 t1ada ta3dat t1adr 1tafe
    t1afg 2tafl t1afr 2ta1fu 1t2ag ta4ge tag1ei t3agent tage1r tage4sp ta3gla 2t3ago
    2ta3gr tag4st 3tagu tahl3sk tai2 tain4m ta3ins 1tak 2t1aka t1akk ta3ko takt3a
    t2aktu ta2la tal3au tal3d ta2le tal2en tal3end tal2ga t2a2li tal2l t1alm. 2talo
    ta1loc tal1op 4talt tal1tr ta2lyt tam4b t2amen ta3mir ta1mo t1ampl 2t1amt ta1mu
    t1ana tan3ab t3anal 2t2and tand4ar t1anf 2t1anh 2t4ani 1t2ank 2tanl t1anm t1anna
    1tanne t1anse t3ansi t1ansp 1tanzg t1anzu tan2z1w ta2pf ta1pho ta3pla t2appe
    ta2r 2tar. t1arb tar3bl 2tarc 2tare tar3er ta3ric 3tarif 2tark t1arm tar3ma
    tar3sc tar3sk 2tart t1arti tar3u t3arw t1arz 2tas. 1tasc 2tase ta4si 1task tas4m
    t1asp ta1st2r 2tasy ta1tal tat1an 2tat1ei tat1er ta1the 2tati t3at4l t4atm ta2to
    4tatt tatt3an taub1l 2taud 3taufe. t1aufg tau3f4li 2taufw 1tau2g 2tauge t1auk
    2taur t1ausb 1tau4sc t1ausd 1t2ause t1ausf t1ausg t1ausk t1ausl t1ausr 2t1auss
    t1ausü t1ausw t1ausz tau1z 2tav ta1ve ta2vi 3tax tä2 2täd 1t2äf 1täg täl3c t1ält
    2täm t1ämt tänd3l t1ängs 1tänz 2tär tär3b tär1d tär3g tär3r tär3sc t2ät 2tät.
    tät4s 2tätt 2t1äug 2t3b2 tbe1 tbe2i tber4 tblock5e t1c t5ce2 t3cha tch2i tch1l
    tch1w t3cl t3cr 2t3d2 tde4k t4dem. tde1t tdi4 4te. 1te2a 2teak 2te3al team5m
    2te3an 2teau te1b t4ebb t1eben te3bl te3bü 2tec. 1t2ech 2te1cha 2teche 2teck
    teck2e 2te3ei te1ele te1em teen1 te1erw tee1t tef2 tefe4 te2g t1egg te3gl 1tegri
    2tei. 2tei1b 2teie 2teif tei1gl 2teik2 1teil 3teilc 3teiln 2teim 2tein tein3ec
    t3einge t3einla tei1ra 2te4is t1eis. t1eisb tei3sc tei3st tei1z te2ke te2ki
    tekt2 te3ku te2l tel1a te3lan tel5d2 tel3ebe tel1ec 3telef 1tele3g tel1eh 1telep
    t3elf. tel3fe 2teli tel1in tel3lan tel3lau tel1lä tel3leb tel3lin tel3m 2telo
    te3los te4lost tel1ö tel1s2k telt2 tel1ta tel1tr telz2 2tem. te3mann tem1ei
    tem3ma tem1or 1tempo tem3st tem1um te2n 2ten. ten3a te3nac ten3ä ten3deb ten3deg
    ten3del t1endf t6endi t1endl 2t6endo t1endp ten3d2r ten3dsc tene4 ten1eb ten4ei.
    ten3end ten3ero teng2 t1eng. t3engla ten3in ten1k4 1tenni 3tenor. tens2t tens3th
    ten3tal ten5tant t1entb t4ente ten3tel ten3tim ten3tis t1entn tent3ri t1ents
    t3entw ten3zu te2o te3ob te4ph t1epi te3po te4poc tep3t 4ter. ter3ac te2ran
    ter3as ter3c ter1d t3erde. ter3eif te1rek ter3end t3erfol t4erfr 2ter3g2 t1ergu
    t4e2ri ter3k t3erklä ter3la t1erlö 1termi ter6mine 2tern ter5nest ter3net
    ter3neu ter3nos t4ero te1rom t1er1ö 3terras ter3ren 2ters terst4 t4erst. t4ersti
    tert4 ter3tan ter3tei ter3tel ter1th ter3tie ter1uf ter3v terz2a t1erzb ter1zi
    6tes. tes3ac te3sc te2se te3seg t3esel tes1er te2si te3sig t2es2t tes3tät
    test3ei 3tests te2su 2teß 2tet te3tab te1tau te1tee te3tel te1tie te1tis te3to
    tetra3 te1tu te3tü teu2 1teuf te1un teur3a te2vi 1tex 2texa t1exe t1exi 2texp
    3text t1exz te3z 2t3f2 t4faß 2t1g2 t3gan tgebe2 tge1r2 t3go th2 2th. t1ha t2hag
    3thal. 3thalp t2har t3hau4 2t1hä the2 2the. 3thea 2t1heb 2t1hei the3in 1t2he4k
    1theme 2then 3theo t1herg 1ther2m ther4sc t2hes 1these t2heu 4thi thic5k t3him
    t1hin t1hir th3k th3lä 2thm 4tho t1hoc t1hof t1hoh t2holo t1holz t2hon tho3sp
    t1hot t3hou 2thö thr2 2ths th3sc t1hu t2hum 1thur t1hü 2ti. 2tia4 ti1ag ti1alk
    ti1all ti1am 2tib ti3ba tibe2 ti1br ti1chr 2tid ti1di ti1do 2tie. tie3br tie3d
    3tief. tieg4 tie3i 4tiel tiel3a 1tierc 3tiersc tier5tel 2ties 2tieß ti1eu 2tif2
    ti1fe ti1fr ti2g ti3gene ti4go ti3gr 2ti3h 2tii ti3is 2tij 2tik tik2el ti3ko
    tiks2 tik1t ti2le 1til2g til3ger til1lo til3m ti2l1ö til1t til3v tim2 ti1maf
    ti1mil t1imp t1ind tin2e t1inf ting1a ting1l tings2 ting3st t1init t1inj tin2k
    tink1l ti3nom t1inse 1tinu t1inv 2tio tio2c 1tip. ti1per 1tipp 1tips 2tiq ti1ra
    ti1rh 2tis. tisch3w 2ti3se2 tis1ei tis4fa ti3sk ti3so 2tiss ti3sta t1iß 2tit.
    ti1tal 1tit2e4 3titel 2titen ti1ter ti1the 2tiu tium2 2ti2v tiv1o tiv1r 2tiw
    2tiz tiz3z 2t1j 2t3k2 4t1l t3lä tl2e t3lebe t3lei t3lek tle3me t2len tler3a
    t3lese t3let t3li t4lieb. t4li5f t4lingi tlings3 tli3sc t3lo t2lu t3luf t4lun
    tlung4 t3lus 2t1m2 t3ma t3mä t3mei tmen4 t3mi t4mig t3most t3mot t3mus t3my
    2t1n2 tnach4b t3nad t2nan t3näh t2nes tness1 t3nö t3nu 4to. to3at to3bat 2tobl
    t1ochs 1tocht t4od tode4 tod1er todes1 2to1di to1do tod3u 2tof2 to1g to3ge 3togg
    to3gi to1ho to3ja to1k2 2told tol3k 2tolo 2tolt t4om to1man to6mere. tom1t t2on
    2tonc ton1d 2tonei 1tonn 3tonst. 1tont 4tontei ton1to 2tontu 2tonum to1ny to3om
    to3pap to1pec to4pfe 1to2po 2to4pt top1te to1q 2torc 2t1ord to1rea tor1el 2t1org
    2tori to3ric tor3int tor2k tor3ka tor1m tor2n to1ro t1ort. tor1ta 2tory to3sa
    to3sc tos2e to1ser tost2 to5sth to1str to1tr tots2 2tott 1t4ou to3un to3v to1wa
    1toxi to3z 1töch 2t1öf 2t1ök töl2 1tön tön1c tön4l tör3b tör3g tör3p tör3s 2tös
    t1öst 1töt 2t3p4 tpf4 t1q tr4 2tr. 2tra. 2traa 2traä 1trach t1rad. 2trafa 3trag
    4t1rahm 3t4rai 2trakl 2tra3ko 2tral tral1l tra3na 2tran4d 3trane 1trank 3trans
    2trap tra3sc t1rase t1rasi 2trasp tra4st 2traß trat4s 2trauc t2raue 2traup 2trav
    2tra3z 1trä2 t1räd 3träg 2träh 3träne 4träts 2träus 2träuß 4tre. 2tre2b 2trec
    t1rech t2reck 4t1red 3tref 4trefo 2t3re2g tre5gist trei2 1t4reib 2treif t1reig
    4t1reih trei4k t1rein 2t1rei4s 2treit t1reiz 2trek 4trel t4re2m tren2 1trend
    t1rent 1trep 2trepe t3repo 3trepp t4repr t1rese 3t2ret tret1r t3rett t2reu
    2t1rev 2trez 2tré t1rh t2ri tri3co 3trieb. 3triebs trie3c trie3d 6trieg 2trigi
    tri3ma t3rind t3riß tri3tiu 2tro. tro1b 1troc tro1ch 4trock. 2trod 4troh tro1ha
    tro1he 2trok tro1li tro2mi 3tropf 2tropo tro3r 2tros tro3sc tro3sm 2troß 2trou
    2trov 2t3röc 2t1röh 2tröm 1tröp 2t1röss 1tröt 3trug 2truk trum2 trums1 t1rund
    3t2rup t1ruß t1rüc t2rüg 2try 2ts tsa2 ts3ab t3sac ts1ad t2s1ah t3sai ts1al
    t4s1amp t4s1amt t2san ts3ane t3sani ts3ar ts1as t2sau t3sau. t2säh t2s1än ts2bur
    t3schan t6schart tsch5aus t4schen tsch4li tsde4 t1se2 ts1eb tsee1i t3seg t3seil
    t3sek ts1em ts1ene ts1eng t3sens t2s1ent t3seq t2s1er t3serv t6s5essen t3set
    t2s1ex t1si t3sic t2s1id t2sind ts1ini t2s1ir t3sit ts3k t3skala ts3li ts3m t4sn
    ts1o tsof3 t1sol t1som t1sou t1soz t2sö t3spal ts1par ts4pare t3spat t2spä
    t3spek ts1per t2s1ph ts1pis ts3ple t2spo t3spon t3spor t4sprei t3spru t2spun
    ts3s2 t1st4 t4s3tabe t2staf t4stag t4stanz ts2tau ts3täti t2stea t3stein ts1tep
    t3stern t3stero ts3terr ts2teu ts3th ts1tie t3stil t2stip ts1tis t4s3tit t4s1ton
    t3stop ts1tor ts3trad t2strä t4streu t2s1tri ts2tro t4strop t2s1trü t3stu
    ts3tuns t3stü ts1u ts3un ts3ut t3sy 4tt tt1abe tt2ac tt1ad t3taf tt1ap t3tari
    tt1art t1tat tt1auf t1tä t3tät tt4bu tt1ebe t1tei tt1eif t3teig tt1ein tt1eis
    ttel1o tte4n tte2r tter3f tt2erg tter4ra tte2s ttes1ä tt2ga tt2häu tt1ho t1thr
    tt3la tt2le tto1b tto1m tto1w t1tö tt1rea tt1ren tt1res tt1rol tt1rud tt1rü
    tt4scha tt2sen tts1p tt2spe tt2spr tts3tem tt2sti tt1sz tt1t2 tt2un t1tut t1tü
    tt2wil tt1z tu2 2tu. 2tua tub2 tuba3b 1tuc 2tuck 2tud 2tuel 2tuf2 t3ufer 2tuh
    2tui t3ukr tul2 2tulin t2um. 2tumb t3umf 2t3umg 2t1umh t3umk 2t3umr 2t3umt
    2t1umw t3umz 1tun. t1una 2t1und t2une t3unf 2tun2g t3unga tung4s3 tu3nic t1unio
    3tunn t3uns t3unt t1up. 2tur. 2tura tur1ag 2turbe 2tur1c tur1d 2ture tur1er
    tur3ki tur3men tur3r 2turs tut4 1tux 2tüb tüb3l 1tüch tück2s 1tüf 2tüh 1tür.
    tür3c 1tür3g 1tür3s 1tüte 2tütz 2t3v tver3g 4t1w twei2 t2wo2 1t2y 2ty. ty3d ty3e
    ty1g ty1k 2tyl ty1mu ty1n 3ty2p typ1t 2tyr ty1rä 2tys ty3sc ty1se ty1st 2tz
    t2z1a tz3an tz1ä tz1ec tz1eie tz1eis t3zell tz2ene tz3entg tz3ents t3zim tz1imp
    tz1int t3zir tz2lig tz1ob tz1of3 t3zol t3zon t2z1or t1zö tz1ti t3zuh tzüg2 tz1wä
    tz1wi tz1wu tz1z 2u. 2ua u1ab uack3 ua3ku ual2g ual1k ual1tr u1am ua1ma uam3p
    uan2d uan1s ua2p ua3pl ua2r uasi3 uat2i ua1tin u3au uä2 u1äm u1äu 2ub u2bab
    uba3k ub2bi u1bec u1be2e u1beg u1bema uben3so u3berat uber3b u4bern u4bers u1bez
    ub4i2 u4bis ub1läu u1blö ub1lu ub3lun u2bö u1br ub1rit ub2san ub4s3che ub2so
    ub2s1pa ub2s3z ub3um u1bur u2büb ub3z 2u2c uch1a uch3an uch1ä uch1ec uch1ei
    u3chem uche4r uch1il uch1in uch1la uch1le uch1li uch1lo uch3m uch3n uch1op uch1r
    ucht3re uch3ü uch1w uck3erl uck4err 2ud2 u3dam u2dat ude1e ude2l uden4te u1de2p
    uder2e ude3sa u1dez udge4 ud3h ud3on ud3ra u3dru ud3we 2ue ueb2 ued2 u1ef ue1g
    u1ei ue2j uel2l uel1t u1emi ue2n uen5a uen2gl uen3k ue2r uer3a2 uer5at uer1ä
    uer3b u3erbau uer1d uer5eife uer5g2 u3erhal uer3m u3ernan uern3st uer1o u1erö
    u1erri uers2k uert2 uer3tei uer1th uer3v u1eta ue4te ue1th ue1to u1ex uf1ad
    u3fah uf1ak u3fal uf1ar u3fas uf1au uf1äs uf1äß 4ufe. uf1ei uf1em u3fenst
    ufe3rat uf1erh uf1eß uf1eur 2uff uffel2 uf1fl u3fic uf1l uf3n uf1ori u1fö uf1r
    uf2spo uf1tel uft3erd uf1tie uft3s2 uf1tu uf3z 2ug uga4 u4gabte ug1af ug1ak
    ug1ap u2g1ar ug1asc ug1au ug3be ug1d2 u3gef u2g1e2i ugen3g u3gep ug1erf uge3rie
    ug1erl ug3f ug2g ugge2l u4gierd u2g1la u2g1lä u2g3lo u1glö u2g1lu ug3m ug4men
    u2gn ugo3 ug1or ugre3g u4g3reis ug1ro u2grol ug1rum ug1rüs ug3sau ug3se ug3sim
    ug3spa ug4spr ug4spu ug3str ug3stu ug3s2tü u1gui u2gum u1ha u1hä uh2d uhe1g
    uhe1h u1hei uhe3k uhe3ma uher2 uhe3ra uh1la uh1lä uhls4 uh1ma uh1mi uh1na uh1nä
    u1ho u3hö uhr1a uhr3er uh1ri uhr1in uh2r1o uhr1te uhs2 uh1se u1hü uh1w 2ui uil4
    uin1g ui3no uin4tu ui4s uis2l uit4 ui1v u1j uk2a u2kaly u1kau uk2e u1kera u1ki
    uk2k uk2l u1kla u1klot u1klö uk2n uk2r ukt1in ukt1r uk2ü uk2z ul1ab ul1am u3land
    ul1äm ulb2 ul1c ul1die ul2dr uld2se 2ule u1led u3lei ul1el ul1erh uler2s ules3t
    ul1eta ul3fe ulg2 u3lic ulik1t ul1ins u1liz ul3ko ul1ku ull2m ull1s ul1n u1lo2
    ul1or ul4p1h uls1a ul3su 4ulta ult3ar ulte2 ult2h ul3ton ul1tro ult3se ul2v ulz2
    u1mac um1ak uma1li um1all um1am uman1g um1anz um1ar u1mate um1aus u1max u2m1äh
    umär4 um1ärg um3ba um2bo um1d2 2ume u1mec umen3st umer2a um1erf um1erg um1erl
    um1erw 1umf 1umg um1inh u2m1ins um1ir 1umk 1um3l 2umm umm2a um3mas um4mes um2mu
    um1n u3mol u1mon umpf4li 1umr 2ums. 2umsab 3umsat ums1er 2umsf um2sim 2umsk
    um2s1pe um1st umt2 um4tat um1th um1ti um3um um2un u2m1ur 1umz 3umzug u2n 2un.
    2una. 1u4nab un1ac una3ga un3an1 un1ap 2unas u4n3at una1ta un1ä un1c 1undd
    und3erf 2undg und3l 1undn un3dot 2un2d1r 4unds. und3sa und3sp und3st und1t
    und1um 1undv und4weg 1undz un1e un3ein un3eis uner3g un3erz u3net unf2 un5fa
    unft4s 2ung 4ung. un2gar 3un3get 3ungew ung5h un1gl 3unglü ungs1 ung4sa un2hof
    un1ide 1unif un1in un3isl uni1so 3u4niv 2unk unk1a unk1n unkom1 unks2 unk3si
    unk2ti un4meh unn1ad unn2e un1o 1unr un1s2 unsch5el un4sere un3sin un3sp uns4t
    2unsy 2un2sz 1unt un1ta un2te un3tei 2un2th 2un1ti 2un1to unt4r unt3sa 2untu
    2un1u4 u3nut unvoll3 1unw 2unwä 2unz2 2uo u1ob u3of uof3f uor1c uor2g up2 u1pa
    uper1 uper3c uper3m uper3r u2pf2e u2pf1i u1phi u1phon up4i u4plet u1po u1pr
    upra3 up3si upt3a upt3erf upt3erg upt1o up3z u1q 2ur. u1rad ura3k ur3alg ur1am
    ur3amt ur1ana ur1ang ur2anh ur1ans ur3ar ura3sc ur3asp ur3att ur1au ur3auf 4urä
    ur1än urb2 ur3bet ur2bi ur2bo urd2 ur3dam ure3b u1rec u1red ure1e u1ref ur1eff
    ur2eg ur1e4p ur1erw ure2s ure2t ur1eta ur2eth 2urf urf1li urf4spr ur4gi urg3st
    ur3h ur1imp uri3mu ur1ind ur1ini ur3ins ur1int urio4 urk2 ur2ke ur3kel ur2ki
    ur3kl urk3t ur3l ur5la ur4lige ur1ma urmen2 ur3mi ur1mo ur2n ur3nam ur3nat urn2e
    ur3no 2uro uro3b uro1c uro1d uro1g uro4kr uro1m ur3or ur2os uro3sc ur3p ur4pat
    ur4pi ur4pu 2ur2r ur3ra urre1c ur3ro ur4rou ur3sac ur3sal ur2sk ur3son ur4spa
    urst4r ur2su ur4sw urt2 ur1ta ur3tei ur1tem ur3th ur1tie urt3sc ur1tu u1ruc
    u1rui u1rü ur3va ur3ver ur3vi u3ryt ur2z urz1a ur4zac urz1o urz1w 2us 4us. u2sal
    us4ann us3art us3chec u3schei usch3mü u4schun usch5wer us1ec u1see us1ei u3seid
    usen2d usen4tu use1ra us1erk us1erl us1ers us1erw us1ese us1ex usi4 u3sim uski4
    us3ko us3m us3oc us1oh us1op us1ou u1sov us2par us3part us1pas u3spek us3pic
    us1pu uss3aue us5sendu uss5erfa uss5ersu uss3k us1so ussof3 uss3tab uss3tät
    uss3tri us2sü ust3abe us3tag u3stal us1tar us1tas us1tau u3stel usten3t us1tor
    u5stras u3stu us1tur u3stüc us3ty u3sum us2ur u2sü us3w u3sy u4s3z 2uß uß3o
    ußof3 uß3ri uß1u 2ut u2tab u3taf ut1alt ut2am ut1ap ut1ar ut3ara ut1är u1tät
    u1tec ut1ed ut1ege ute3gi ute3gr u1tei ut1ei. ut1eie ut1ein u2tem u2ten uten4a
    uten1e ut3ersa ut2es ute2st u1teue ut1ex1 ut1hi ut1ho u1tit ut3ne u2to1 uto3c
    ut1opf utor2a u1tö u2töl u1tras ut1rea ut1rü ut2s ut3s2a ut3sä ut4säu ut3se
    ut4sh ut3s2i uts4ka ut3so ut3sp ut4spa ut3st ut2ter utt4l ut2to1 utto3r ut1tr
    utts2 ut1tu ut2z utz3eng utz3g utz1in utz1w 2uu u1uf uum1 u1uni uur1 u1ü u1v
    u2vi u3vie u1w u3wä 2ux ux2e ux2p uxt2 2uz uzer2n uzin4 uz2li uz1ot uz1we 1ü2b
    üb1ä 2übc 2üb2d übe2 über3 über5h üb1l üb2le üb1ro üb2st 2üc üch1l üch2s üchs1c
    üch2t ücht4e ück1er ück3eri ück1l ück4spe ü2d üd3a4 üd3d üden2 üd1o2 üd1ö üd1r
    üd3s2 üd1t2 üe4 ü2f üf1a üf1ei üf1erg üf2f üf1i üfin3 üf1l üft2e üft2l ü2g üge1g
    üge4l üge2r üg4g üg1l ü3gr üg3s üg4st üh3a üh3b üh1ei üh1eng üh1erf üh1erk
    üh1erz üh1i ühl3ag üh1lam üh1lä ühl2e üh1li üh1ma ühmen2 üh1mi üh3mo ühne3t
    ühn2s üh1o ühr2e üh1rei ühr3ei. üh3rent üh3ro ühr3ta üh1s2 üh3temp üh3t2r üh1w
    ü1ka ül1a ül2e ü1lei ül1k üll1au ül2le ül2lö ül2p ül2v üm2m 2ün2 ün3a ünd3ler
    ün3do ünd3s ün3ei ünf1ac ünf1ei ün3fl ünf1r ün3gü ün3ker ün3ko ün3r ün3se üns2l
    ün3sp ün3str ünte2 ün3tee ün3tö ün3zo üp2 üpel1 üpf3l ü1pu ü2r2 ür3a ür3bo ür3ei
    ür4f1r ürg3eng ür4gi ür3gr ür3h ür4ke ür3kl ür3na üro1 üro3f üro3g üro3h üro3sc
    üro3t ürr2 ür3ra ür3sc ür3se ür3si ür3so ürt2 ür3v ür3za ü2s2 üsen3 ü3sp üss1c
    üss2e üt1al üte1d üte3g üte1m üte4n üten3t üt1r üt2s1 ütt2g üt2z ü1v 4v. 2va.
    v1ab 1vag 4vai va4k2 4vald val2e val4s va1ma van4c 2vang van2n 2vap v1arb vas2
    va3st v4a2t vat3a vat3h vatik2 vat1in vat3m vat1r vat3s4 vat1u v1au v1b 2v1d ve2
    4ve. 2vea ve3ar 2ve3b ve3c 2vef ve3gr veil2 v1ein veit4 veits1 2vel ve3lan
    vel1au ve3lei velt4 2vem4 ve3ma ve3mu 4ven. 4vend ven3k 1venö ven2t 2veo ve3of
    2vep ver1 4ver. 1ver3a ve3rad ver3b2l verd2 ver3da veren2 verf2 3verg2 ver3m
    ver4mon vern2 ver3r ver3sta ver2tü ver3u ver3v ver4ve 3verw ver3z ve3sa ve3sc
    ve3se ves2p ves2t2 2vet2 ve3ta ve3tr 2vev ve3vo 2vew 4vey v1f v1g v1h vi4 2vi.
    via4g 2vic vid3st vie4 vieh1a vier1d 3view 2vii 3vik2 vil1a vil1eh vil1in vil1se
    v1im vin2 v1int vinz3l 1vio 3vir vis2 vis4z vize3 2v1k 2v1l2 v1m4 2vn 1vo2 4vo.
    vo3a 4v1ob vo3bl vo3br voge2 5vokatu v2ol voll7auf. voll5end 4vo3m von1 v1op
    vor1 vor3a vor3b vor3da vor3di vor3e vormen4 vor3p vor3z 4vos voß2 3voti vot1t
    2vou völ2 2vös2 4v1p 2vr2 v1ra v2ree v1rei v1ro 4vs v1se2 v1si v1so v1su v1s2z
    4vt v1ta v1tä v1to v1tö v1tr vul2 v1v v1w 2vy1 v1z 4w. wa4 2wa. 1waa wab2 wa5ba
    wa5bo 1wag 3wagen 1wah wahl5ent wai2 waib2 2wak wal2bu wal2c 1wald wal2k wal2t
    walt3a walt4st 3wal2z wam4 wan2 2wana w1an3f 2wang wang4s 1wann wan3se wanz5end
    war4 2war. 1ware ware1i 3wareke ware1l waren3 wart4e 1was2 wasch3l wasi3 wass4
    wä2 1wäh 1wäl 2wäng wär4 1wäs w1b w3bi w2boy 2w1c w1do we2 4we. weat3 we4b web1a
    4webeb web1l web3se wee2 weed1 1we4g weg1a weg5ersc weg1l weg1n weg1r wegs2
    weg5sei weg3sp 3weh weh1l weh1m weich3l 2weie wei2er wei3k wei1m wein4sa wei1p
    wei3sc weis4se weis4s3p wei3str wei2tr wel2 wel3b well2s wel4t1 welt3a welt5end
    we3me we3mo wen3a wen2d wen2g wen3ge wenk3ri we4r2 wer3a 1wer4bu werd2 5werdens
    1werdu 2werg werg1o wer3i wer4in. 1werk. 1werke wer3l wer3ma wer3me wern4h
    wer3ni 2werp wer3r 2wers wert3a wert3ei wert5erm wert1o wert3ri wer3v wer4wo
    we4s 1wese2 wesens3 wes4t west1a west3ei west1o west1r 1wet 4wete 4weto 2wets
    wett1s 4we3u 4we3v 4wey 2w1g w1ho w2i2 3wid2 2wieb2 wie3m wien2e wien1t wie4st
    4wig wig4s 2wik2 1wild wil2f wil4h wim4 win4 4win. wind3ec 2wing 4wini winn7ersc
    wire1 1wirr 1wirt wis4 1wiss wit2 4witzd w1ka 2wl 2w1m wn1sh wn1t wo4a wo2b 1woc
    wo4d wo4f 3woh2 wo1he 1wolf wolf2s3 won2 wo1na wor4 wort1r wo1si wo2v wo2z 1wöc
    wöl4 wör4 wört2h 2w1p 1wr wrest4 w1ro 4ws w2sc w1s2h w2s2k w3sky ws1s wsser2 2wt
    w1ta w1term w1ti wton2 w3tons. w1tr w2u2 1wuc wuch2 wul2 wul4f wuls2 wul3se wun4
    wung1r wur2 4wur. wur4f wur3g wur4l 1wurst wus2 1wut3 1wüh wül2 wür4 2ww wy4
    2w1z x1a xa3by x2a3d xa1fl x2a3g2 x2an xan2d x3ann x4ant2 x4anz x2as x1b x1ca
    x1ce x1ch x1cl x1d x2du xe4 x1eges x3egete x1em xem3b x2en xen1k xens2 xen3sa
    xen3t xer2 xer3a x2ere xers2 x3eu x1f x1g xge1 x2gl x1h xi1c xich2 xid1em x1ido
    xie3l xi1g2 xil1l xil3u xim2 xins2 xin1sk xio2 xi1r xi4s1e xis1o xis1s x1itu
    xi1ze x1j x1k xkal2 xl2 x1lä x1le x1m x1n xo2d x1oe x1or2 x1ö x1p xpan4 x3pe
    x3po x3pr x1q x1ri x2ro x3s2 xst2 x2t1a xt2ant xt3anz xt2as xt1ä x1tät xt1ed
    xt1ei xt1erf x1term xt1ev xt1h x2ti2 xt1ill x1to xtra3b2 x4trag x1trak xt1ran
    xtra1t x2tre xt1re1c x1tru xts2 xt1t xt1um xt1un xt3w xt1z xu2 xual1l x1un xus3
    x1ve x1w xy2g xy1m xy1r xy1t x1z y1ab y1am yan2g yan4m y1ät y1b2 y1ca yco3b
    ycon2 y1cr y2d2 y3dec y3do ydro1 y1ed y1ei y2el yen2 yes4 y1est y3f yg2 y1ga
    y1ge y1gl y1go y3gr y1gu y3h yhr2 y1in2 yja2 y1ki ykol3 ykot2 y1ku yl1am y1lau
    yl1c yl1d yl1es yl2l yl3lan yl4lo yl3m y2lo ylo1g ylo1p y1lö yl3sa yl1st y1m4a
    ym1m ym2n yn1 yn2a y1n2u yo1k yom2 yon4 yo1p yor2 y1ou y1pa yp3ab yp1an y1pä
    yp4e yper1t yp1f y1pho yp1id yp1in y2p1l y2po3 y4ps yp1th y2pu yp1um yr2 y1rau
    y1rei y3rig yrin2 yro1 yro2p yrr4 y2sa y2s2c yse1e yse1g y3ser y3s2h y4sl ys3la
    y3s2pa ys2s yst2e y1sty ys1u y1s2z y1ta y1tec yt2h y1tu yu2 yur2 y1v y1w y2z
    y3ze y3zy 4z. 1za 2za. 2z3ab 3zac z1achs 2z1ad 2z1af 3z2ah 2z3ak 2z1al z2ali
    2z1am za1ma 2z1anf 2z1angs 3z2an4k 2zanr zap2 z1arb za1res 2z1arm z2aro 2zarti
    2z1as2 za3st2 2z3at4 zau2 3zaub z1auf z3aug 3zaun 2za1v 1zä z1äc 3z2äh 2z1äm
    2zängs z1ärg z1ärm 4z1b2 zbe1 zbe2i zbe2to z2bol z1c z3ch z1d2 zde2 z2dem zdi3st
    z3dü 4ze. ze2a ze3ad ze3am ze3au 2ze1b z1eben ze2d ze3di ze1e 2zef z1eff 2ze1g
    3zehe zehen1 3zehnm 1zei zei1b zei1d 2zeie zei2g zei3k2 zeil2 zei1m 4z1ein
    z2e1ind ze3inse zei1r zei3s2 zei1sk zeiss2 zei1st4 zeiß2 3zei2t zeit1a zeit5end
    zeit3er zeit3ri zei1z ze2l 4zel. zel1a zel1d zel1er zel1in zel2l 2zell2a zel3lau
    2zel1lä zel3läu 4zelleb 4zellei 4zelliz 2zel3lö zelm4 zels2 zel1sz 2zem4 ze1mi
    4z1emp ze2n 4zen. 2zena zen1ac 4zene 4zeng 4zenh zen1k 2zenn zen1ne zens2e
    4zensio zen5str zent1a z1en1th zen3tis 8zentralit zent3sk 4zenz zenz3er zeo1l
    ze4ph ze2pi 2zepl 2zer. zer1d ze1rek 2zerg z1ergä z3ergeb z3erhal 2zeri 2zerk2
    z3erlau z1erlö zer3m zer4mat z2ern zer2ne zern3ei 2zerq zer2ru zers2 z1ersa
    zer8schneidu zert1a zert3ag zert4an zert5rau 2zerw z1erzi ze2s ze3sc 2zesk
    ze3sku zes2s zess5end zess1t ze3sta zes3tec zes1tr zeß3 1zet 2zet. 2zeta ze1ti
    2zett. 2zetts 3zeu2 z1ex 2zez 2z3f2 z1g zge2ni zger2a z2gere zge5rinn z2germ
    zger2s1 zgie4 4z1h zher2 zi2 2zi4a2 2zi3b 1zic zich2 zid1r zie4 3zieh 5ziehs
    5zieht 1ziel. 2zien 2zierk zie5sc 2zig zi3go zi3h 2zil4 zi3la zi3lit 1zim2 zi3me
    3zimm 2zimp zimt3 2z1ind z1inf z1inh zin2s zin3sc z1inv zi4o zio1d 2zipr 3ziproz
    1zir zi3re zirk2 zirk4s zis4 z3iso ziss4 zi3sz ziß4 2zit. 2zitä zit2h zit1o 2ziu
    ziv2 zi3z z1j 4z1k2 zkom1 z1l z4laue z4laus z3leb z3led z3leg z3leh z3lein
    z3leis zle3t z2li3p z2lu z3luf z2ly 4z1m2 2z1n2 znach4b z2ne znei3 z3nel z3neu
    1zo 2zo. 2zo1d zo2e 2zof zo2g 4zogenb 4zogenr 2zogi 2zogl 2zogs. 2zogsm 2zogsw
    2zogtu 2zogtü 2z1oh zo1k 2zol. 2zola zom4 zo1ma zonal2 zo2o zoo3f zoo3t zoo3v
    2z1ope zo3pi zor2 2zos2 zo3sc zo1su z1osz z1öf 1zög z1ök z1öle 1zöll 2zön z1p4
    z1q 2z1r2 4z1s2 z3sa zse2 4zt z1ta2 z2t1au z1tä z1tec z3tee zte3g z1tei zt1ein
    z1temp zte3o z1term zte3str z1teu zt1h z1ther zt3ho z2tin zt1ins z1to2 z1tö z1tr
    zt1rec zt2ru zts2 zt3se zt1so zt1t z1tu z1tü zt1z 1zu1 zu3a 3zu3b2 3zuc zuch2
    zuck4 zud4 zue2 zu2el 3zuf2 3zug zugs1 zug3un z1uhr 3zul 3zum 4z1um. zum2a
    zumen2 4zumr z1ums 2zun2 zu3na zu3nä 3zu3ne 3zunf zung4 zu3ni zu3nu zu3nü zur4a
    zu3re 2z1urk 2z1url z1ur1t 3zu3s2 zusch4 3zut4 zu3to zu3u zu3v 3zuw zu3z2 1zü
    2züb 3züg 3zün zünd4le 2z3v 1zw2 2zwag 2z1wal zwand1 2zweg zweg3s 2zweh 2z1wel
    2z1wen2 z1wer4 2z1wes z2wic zwie3g zwie3l zwil1 z2wit 2z1wo z1wör z1wur 2z1wü
    1zy 2zy. 2zylb 2zyle 2zym 2zz zz4a z1ze z3zei z3zet zzi3s zz2l zz3la z1zö z3zu
    zzug4 z3zw
    """)

    def patterns(self):
        """Invariant: patterns() is of type “unicode”.
        Invariant: patterns() provides patterns for the hyphenation algorithm.
        These patterns do not break at hyphenation points, but break where
        ligatures have to be suppressed in german texts. Example:
        auffallend → auf fallend
        The patterns are all lower case.
        """
        return GermanLigatureSupport.__germanLigaturePatterns

    def get_word_characters(self):
        """Invariants: Returns a “unicode” value that have to be
        considered as word characters when using the patterns.

        This is the content of the file “german.tr” of the Trennmuster
        project (both capital letters and small letters), adding LATIN
        CAPITAL LETTER SHARP S (ẞ) and LATIN SMALL LETTER LONG S (ſ). So it
        contains all characters that occure in the original word list from
        which the patterns are generated. (So it is likely
        that is contains more characters than the patterns itself.)

        It contains also at least for the pattern itself (not necessarily
        the hole word list) every character to represent every normalization
        form.
        
        NOTE There is no special treatment for Unicode normalization forms.
        
        WARNING This function must be kept in synch with
        simple_case_fold_for_lookup() and also with
        InstructionProvider.get_instructions() which does some Unicode
        normalization.
        """
        return (u"aAäÄâÂàÀáÁãÃåÅæÆbBcCçÇdDeEéÉèÈëËêÊfFgGhHiIíÍìÌîÎjJkKlLmMnN"
                u"ñÑoOóÓòÒôÔöÖøØœŒpPqQrRsSšŠßtTuUúÚüÜvVwWxXyYÿŸzZžŽ"
                u"ẞ"  # U+1E9E LATIN CAPITAL LETTER SHARP S
                u"ſ"  # U+017F LATIN SMALL LETTER LONG S
                u"\u030C\u0302\u0308\u0301")  # combing chars (normalization)


class InstructionProvider(GermanLigatureSupport):
    def __init__(self):
        GermanLigatureSupport.__init__(self)
        self._myHyphenator = Hyphenator(self.patterns())

    def get_instructions(self, my_word):
        """ Get instructions for a single german word “my_word”.
        Precondition: my_word is of type “unicode”.
        Postcondition: Returns a list with as many elements as my_word has
        elements. Each element is either True, False or None. True means
        that at this index position, a ZWNJ has to be introduced, making
        all characters starting from this index position shifting to the
        right. False means that at this index there is a ZWNJ and this
        ZWNJ has to be removed. None means that nothing has to be done.
        Of course, the list might be emtpy if my_word is empty. my_word
        is supposed to contain a single word, and not various words with
        whitespace. This function handles correctly the soft hyphen
        U+00AD and the following characters with canonical decomposition:
        šâäéóöü
        :rtype: list
        """
        if type(my_word) is not unicode:
            raise TypeError("myWord must be of type “unicode”, but it isn’t.")
        stripped_word = u""
        stripped_word_index = []
        folded_word = self.simple_case_fold_for_lookup(my_word)
        # Handle normalization…
        #
        # The string is yet folded, so only small letters need to be
        # handeled. We substitute the decomposed form by the composed
        # form (that is used in the pattern). To avoid a different
        # character count, we introduce a soft hyphen. This is okay,
        # because the soft hyphen will be ignored later anyway, and
        # ZWNJ are always inserted before normal characters and never
        # before soft hyphens. We handle only the minimum of
        # canonical composition that really occures in our pattern.
        #
        # WARNING: This must be kept in synch with
        # GermanLigatureSupport.get_word_characters().
        folded_word = folded_word.\
            replace(u"s\u030C", u"š\u00AD").\
            replace(u"a\u0302", u"â\u00AD").\
            replace(u"a\u0308", u"ä\u00AD").\
            replace(u"e\u0301", u"é\u00AD").\
            replace(u"o\u0301", u"ó\u00AD").\
            replace(u"o\u0308", u"ö\u00AD").\
            replace(u"u\u0308", u"ü\u00AD")
        for my_index in range(len(folded_word)):
            my_character = folded_word[my_index]
            # Do not copy SOFT HYPHEN and ZERO WIDTH NON JOINER
            if my_character not in u"\u00AD\u200C":
                stripped_word += my_character
                stripped_word_index.append(my_index)
        hyphenated_word = self._myHyphenator.hyphenate_word(stripped_word)
        # correct_zwnj_positions will contain the indexes of alphabetic
        # characters before which a ZWNJ
        # should be (index relative to my_word).
        correct_zwnj_positions = []
        i = 0
        # For all partial strings but the last one:
        for j in range(len(hyphenated_word) - 1):
            i += len(hyphenated_word[j])
            correct_zwnj_positions.append(stripped_word_index[i])
        # Make a list with one element for each element in my_word.
        # None means “do nothing”, True means “insert a ZWNJ here,
        # and False means “delete this ZWNJ”.
        my_status_list = []
        # First step: remove all ZWNJ, leave the rest without change.
        for my_character in my_word:
            if my_character == u"\u200C":
                my_status_list.append(False)
            else:
                my_status_list.append(None)
        # Second step: cancel the remove where not necessary and
        # add ZWNJ where necessary.
        for i in correct_zwnj_positions:
            if my_status_list[i - 1] == False:
                my_status_list[i - 1] = None
            else:
                my_status_list[i] = True
        return my_status_list


def is_bmp_scalar_only(my_string):
    """In Python 2, the data type “unicode” is supposed to be a unicode
    string and is a sequence of code units. The encoding form might be
    either UTF-16 or UTF-32, depending on the compile time options of the
    python compiler. What does this mean? Example: The character U+0001F404
    COW might be represented either as a sequence of two UTF-16 surrogate
    pairs (string length: 2) or as a single UTF32 code unit
    (string length: 1). If you want to know if you have to deal
    with surrogates or not, you can use this function. It works
    on both UTF-16 and UTF32 sequences.

    Preconditions: my_string is of type “unicode”

    Postconditions: Returns “false” if the string contains at least one code
    unit that is not a Unicode scalar value or is not within the
    Basic Multilingual Plane. Returns “true” otherwise. Quoting from the
    Unicode standard:
    “D76 Unicode scalar value: Any Unicode code point except high-surrogate
    and low-surrogate code points. As a result of this definition, the set of
    Unicode scalar values consists of the ranges 0 to D7FF (16) and E000 (16)
    to 10FFFF (16), inclusive.”
    """
    if type(my_string) is not unicode:
        raise TypeError(
            "“my_string” must be of type “unicode”, but it isn’t.")
    return re.search(u"[^\u0000-\uD7FF\uE000-\uFFFF]", my_string) is None


def get_affected_text_objects():
    """Preconditions: import scribus
       Postconditions: Returns a list. The list contains the unique object
       identifier string of each affected text frame in the currently active
       document (if any). If there is a set of objects selected, than only
       the text frames within this set are affected. (This might result in
       an empty list if only non-text-frame objects are currently selected.)
       If there is no currently active document, than the list is empty."""
    my_return_value = []
    if scribus.haveDoc() > 0:
        for i in range(scribus.selectionCount()):
            my_object = scribus.getSelectedObject(i)
            if scribus.getObjectType(my_object) == "TextFrame":
                my_return_value.append(my_object)
    return my_return_value


class StoryInterface:
    """An interface to the content of a “story” in scribus. A story is
    the common text content that is shared by various linked text frames.

    Some of Scribus’ script API is about stories, other parts are about
    the text frames, using different indexes which leads sometime to
    unexpected bahaviour. This class offers a simple interface to the
    story content. It hasn’t many functions, but it’s a consistent
    interface and avoids unexpected side effects. It works as expected
    independently of the current text selection, but it might change
    the current text selection. It takes care of the necessary encoding
    and decoding when dealing with scribus, while its public interface
    exposes only functions that work with the data type “unicode”.

    Note: Objects in Scribus (Text frames, images, geometric forms…)
    are supposed to have a unique identifier. However, there is bug
    https://bugs.scribus.net/view.php?id=11926 that allows non-unique
    identifiers. The interaction of StoryInterface with this bug
    has not been tested.
    """

    def __init__(self, text_frame_identifier):
        """Precondition: text_frame_identifier has the data type that is
        used for identifiers for scribus objects and is not empty.
        Postcondition: The object is created."""
        # We assume that the data type of the Scribus object identifier
        # is “str”.
        if type(text_frame_identifier) is not str:
            raise TypeError(
                "argument “text_frame_identifier” has wrong data type.")
        if text_frame_identifier == "":
            raise TypeError(
                "argument “text_frame_identifier” might not have "
                "empty content.")
        self.__identifier = text_frame_identifier

    def read_text(self, first, count):
        """Precondition: The object with the unique identifier “textFrame”
        (constructor argument) currently exists in the current document,
        and it refers to a text frame. “first” and “count” are non-negative
        integers. The requested range exists really.
        Postcondition: Returns a value of type “unicode” that contains the
        requested text range (the total number of “count” indexes, starting at
        index “first”). Note that the indexes are the indexes provides by
        scribus. This may not be unicode characters, but UTF16 code units, and
        if you choose half a surrogate pair, scribus will silently add the
        missing half surrogate pair. The indexes does not refer to the actual
        text content of “textFrame”, but to the content of the underlying
        “story”, that means the common text content that is shared between
        this text frame and all linked text frames. Note that this function
        will (likely) change the current text selection of the story."""
        if (type(first) is not int) or (type(count) is not int):
            raise TypeError("Both arguments, “first” and “count”, must be "
                            "integers, but they aren’t.")
        if (first < 0) or (count < 0):
            raise IndexError("Both arguments, “first” and “count”, must"
                             "not be negative, but they are.")
        if scribus.getObjectType(self.__identifier) != "TextFrame":
            raise RuntimeError("The argument “textFrame” that was given in "
                               "the constructor does currently not refer to "
                               "a text frame in the current document.")
        # If count is 0, scribus.selectText will select nothing. But when
        # nothing is selected, scribus.getAllText will not return an empty
        # string, but the hole content of the story. That is not what we
        # expect, so we have to catch this case manually.
        if count == 0:
            if first >= self.length():
                raise IndexError("“first” is out of range.")
            return u""
        scribus.selectText(first, count, self.__identifier)
        return scribus.getAllText(self.__identifier).decode("utf8", "strict")

    def delete_text(self, first, count):
        """Precondition: The object with the unique identifier “textFrame”
        (constructor argument) currently exists in the current document,
        and it refers to a text frame. “first” and “count” are non-negative
        integers. The requested range exists really.
        Postcondition: Starting from index “first”, the number of
        “count” indexes are removed. Note that the indexes are the
        indexes provides by
        scribus. This may not be unicode characters, but UTF16 code units, and
        if you choose half a surrogate pair, scribus will silently add the
        missing half surrogate pair. The indexes does not refer to the actual
        text content of “textFrame”, but to the content of the underlying
        “story”, that means the common text content that is shared between
        this text frame and all linked text frames. Note that this function
        will (likely) change the current text selection of the story."""
        if (type(first) is not int) or (type(count) is not int):
            raise TypeError("Both arguments, “first” and “count”, must be "
                            "integers, but they aren’t.")
        if (first < 0) or (count < 0):
            raise IndexError("Both arguments, “first” and “count”, must"
                             "not be negative, but they are.")
        if scribus.getObjectType(self.__identifier) != "TextFrame":
            raise RuntimeError("The argument “textFrame” that was given in "
                               "the constructor does currently not refer to "
                               "a text frame in the current document.")
        # If count is 0, scribus.selectText will select nothing. But when
        # nothing is selected, scribus.deleteText will delete the
        # hole content of the story. That is not what we
        # expect, so we have to catch this case manually.
        if count == 0:
            if first >= self.length():
                raise IndexError("“first” is out of range.")
            return
        scribus.selectText(first, count, self.__identifier)
        return scribus.deleteText(self.__identifier)

    def insert_text(self, text, first):
        """Precondition: The object with the unique identifier “textFrame”
        (constructor argument) currently exists in the current document,
        and it refers to a text frame. “first” is a non-negative
        integer ≤ length(). “text” is of type “unicode”.
        Postcondition: The text is inserted at the given index position.

        Note that the indexes are the indexes provided by
        scribus. This may not be unicode characters, but UTF16 code units,
        and if you choose half a surrogate pair, scribus will insert between
        them, making the unicode text string invalid.
        0The indexes does not refer to the actual
        text content of “textFrame”, but to the content of the underlying
        “story”, that means the common text content that is shared between
        this text frame and all linked text frames. Note that this function
        will (likely) change the current text selection of the story."""
        if (type(first) is not int) or (type(text) is not unicode):
            raise TypeError("“first” must be “integer” and “text” must "
                            "be “unicode”, but they aren’t.")
        if first < 0:
            # We have to check for “first < 0” here, because scribus
            # would accept “first == -1” as valid, but we do not
            # want this behaviour here.
            raise IndexError("“first” must"
                             "not be negative, but it is.")
        if scribus.getObjectType(self.__identifier) != "TextFrame":
            raise RuntimeError("The argument “textFrame” that was given in "
                               "the constructor does currently not refer to "
                               "a text frame in the current document.")
        scribus.insertText(
            text.encode("utf8", "strict"),
            first,
            self.__identifier)

    def length(self):
        """Precondition: The object with the unique identifier “textFrame”
        (constructor argument) currently exists in the current document,
        and it refers to a text frame.
        Postcondition: Returns an integer that represents the length
        of the text. The mesurement unit of the length is provided
        by Scribus. It is in Scribus 1.5.2 not “Unicode Scalar Values”,
        but “UTF16 code units”.
        """
        return scribus.getTextLength(self.__identifier)


def show_messagebox(
        caption,
        message,
        icon=scribus.ICON_NONE,
        button1=scribus.BUTTON_OK,
        button2=scribus.BUTTON_NONE,
        button3=scribus.BUTTON_NONE):
    """Shows a message box. Use this function instead of the original
    function to get more exceptions for wrong argument types and be
    forced to use unicode string. (This function uses an explicit conversion
    command for the unicode string instead of relying to the default
    encoding.)

    Preconditions: “caption” and “message” are of type “unicode”. icon,
    button1, button2 and button3 are either not used or of type int.
    Postcondition: Calls show_messagebox and returns the result."""
    if type(caption) is not unicode:
        raise TypeError("“caption” must be of type “unicode”, but it isn’t.")
    if type(message) is not unicode:
        raise TypeError("“message” must be of type “unicode”, but it isn’t.")
    if type(icon) is not int:
        raise TypeError("“icon” must be of type “int”, but it isn’t.")
    if type(button1) is not int:
        raise TypeError("“button1” must be of type “int”, but it isn’t.")
    if type(button2) is not int:
        raise TypeError("“button2” must be of type “int”, but it isn’t.")
    if type(button3) is not int:
        raise TypeError("“button3” must be of type “int”, but it isn’t.")
    return scribus.messageBox(
        caption.encode("utf8", "strict"),
        message.encode("utf8", "strict"),
        icon,
        button1,
        button2,
        button3)


def do_ligature_setting():
    """Do the ligature setting for the stories behind the selected text
    frames.

    Preconditions: Scribus is available.

    Postconditions: Does the ligature setting."""
    def ligature_setting_for_story(identifier, provider):
        interface = StoryInterface(identifier)
        used_characters = provider.get_word_characters() + u"\u00AD\u200C"
        i = 0
        while i < interface.length():
            temp = interface.read_text(i, 1)
            if temp in used_characters:
                # So temp is guaranteed to contain exclusively Unicode
                # Scalar Values inside the BMP. This is because
                # used_characters does so. This is tested (has
                # to be tested!) before this local function is
                # called.
                # Also my_word will have this characteristic.
                my_word = temp
                story_index = i
                i += 1
                while i < interface.length():
                    temp = interface.read_text(i, 1)
                    if temp in used_characters:
                        my_word += temp
                        i += 1
                    else:
                        break
                # It might be better to ignore words, that have in their
                # character style not the german language attribute, but
                # another language attribute. However, I do not know how
                # to get this working.
                instruction_list = provider.get_instructions(my_word)
                for j in range(len(instruction_list)):
                    # Scribus indexes are UFT16 code units.
                    # The instructions use as indexes either UFT16 code units
                    # or UTF32 code units, depending of the Python build.
                    # However, we have made sure earlier that only Unicode
                    # Scalar Values inside BMP are in used_characters, so
                    # it is guaranteed that here the indexes are the same,
                    # wether it is UTF16 or UTF32. So we can savely use
                    # these indexes to do our modifications in Scribus.
                    if instruction_list[j] == True:
                        # Insert here a ZWNJ.
                        interface.insert_text(u"\u200C", story_index)
                        i += 1
                        story_index += 2
                    elif instruction_list[j] == False:
                        # Delete here a ZWNJ.
                        interface.delete_text(story_index, 1)
                        i -= 1
                    else:
                        # Don’t make any modifications here.
                        story_index += 1
            else:
                i += 1

    german_locale = \
        re.search(u"^de(u|@|_|$)", scribus.getGuiLanguage()) is not None

    if scribus.haveDoc() <= 0:
        if german_locale:
            show_messagebox(
                u"Ligatursatz",
                u"Kein Dokument gefunden.")
        else:
            show_messagebox(
                u"Ligature setting",
                u"No document found.")
        return
    all_my_objects = get_affected_text_objects()
    if len(all_my_objects) <= 0:
        if german_locale:
            show_messagebox(
                u"Ligatursatz",
                u"Kein Textrahmen ausgewählt.")
        else:
            show_messagebox(
                u"Ligature setting",
                u"No text frame selected.")
        return
    if german_locale:
        result = show_messagebox(
            u"Ligatursatz",
            u"<b>Einführung</b>"
                u"<br/>"
                u"Einige Buchstaben\xADkombinationen sind eine "
                u"Heraus\xADforderung für die Gestaltung von "
                u"Schrift\xADarten. Ein gutes Beispiel ist die "
                u"f\u2011i\u2011Kombi\xADna\xADtion. Das "
                u"kleine\xA0f ist oft aus\xADladend gestaltet, und so "
                u"ent\xADsteht eine unschöne Lücke zwischen dem kleinen\xA0f "
                u"und dem kleinen\xA0i. Viele Schriftarten lösen dieses "
                u"Problem durch Ligaturen. Die Buch\xADstaben werden "
                u"ver\xADschmolzen; so wird die Lücke ver\xADmieden und das "
                u"Schrift\xADbild wirkt harmonischer. Im Deutschen gibt es "
                u"nun viele zusammen\xADgesetzte Wörter. Ligaturen "
                u"erschweren hier aber das Lesen. Des\xADhalb wird bei "
                u"deutschen Texten <i>in solchen Fällen</i> keine "
                u"Ligatur gesetzt."
                u"<br/>"
                u"<br/>"
                u"<b>Umsetzung</b>"
                u"<br/>"
                u"Ligaturen werden überall dort gesetzt, wo sie nicht "
                u"durch einen Binde\xADhemmer (Unicode-Zeichen U+200C) "
                u"unter\xADdrückt werden. Dieses kleine Skript arbeitet "
                u"sich nun durch alle ausgewählten und alle mit ihnen "
                u"verketteten Textrahmen und ergänzt und entfernt "
                u"Bindehemmer gemäß den Regeln für deutschen "
                u"Ligatursatz. Dieses Skript basiert "
                u"auf einem Wörter\xADbuch, in dem für viele Wörter der "
                u"passende Ligatur\xADsatz hinter\xADlegt ist. Bei Wörtern, "
                u"die nicht im Wörter\xADbuch ver\xADzeichnet sind, "
                u"ver\xADsucht das Skript, den passenden "
                u"Ligatur\xADsatz zu „erraten“. "
                u"Das Skript setzt Binde\xADhemmer an allen "
                u"Morphem\xADgrenzen, und zwar unabhängig davon, ob die "
                u"unter\xADdrückte Ligatur gebräuchlich ist oder in der "
                u"dort verwendeten Schrift\xADart überhaupt vorkommt. Somit "
                u"wird beim Wort „Auflage“ die häufig anzutreffende "
                u"fl\u2011Ligatur durch einen Binde\xADhemmer unterdrückt. Aber "
                u"auch beim Wort „Aufgabe“ wird zwischen\xA0f und\xA0g ein "
                u"Binde\xADhemmer gesetzt, obwohl eine fg\u2011Ligatur nicht "
                u"gebräuchlich ist. Dieses Vor\xADgehen hat den Nach\xADteil, "
                u"dass wohl viele Binde\xADhemmer gesetzt werden, die "
                u"keinen Effekt haben, weil die Schrift\xADart diese "
                u"Ligaturen überhaupt nicht enthält. Dieses Vor\xADgehen "
                u"hat aber den Vor\xADteil, dass es im Grund\xADsatz für alle "
                u"Schrift\xADarten funktioniert – auch dann, wenn "
                u"Schmuck\xADligaturen ver\xADwendet werden oder die "
                u"Schrift\xADart exotische Ligaturen ent\xADhält.",
                scribus.ICON_NONE,
                scribus.BUTTON_OK,
                scribus.BUTTON_CANCEL)
    else:
        result = show_messagebox(
            u"Ligature setting",
            u"<b>Introduction</b>"
                u"<br/>"
                u"Some character combinations are a challenge for type "
                u"design. A good example is the f-i\xA0combination. The "
                u"small\xA0f has often a spreading design, so there is an "
                u"unpleasant gap between the small\xA0f and the small\xA0i. "
                u"Many fonts solve this problem by using ligatures. The "
                u"glyphs are melt together and the typeface is more "
                u"harmonious. But in German, there are many compound "
                u"words.  Now ligatures make reading more difficult. "
                u"Therefore <i>in such cases</i> ligatures are not used "
                u"in German."
                u"<br/>"
                u"<br/>"
                u"<b>Implementation</b>"
                u"<br/>"
                u"Ligatures are used everywhere, except they are "
                u"suppressed with a zero\xA0width non-joiner (Unicode "
                u"character U+200C). This little script walks through "
                u"all selected text frames and all text frames linked "
                u"with them, and completes and removes zero\xA0width "
                u"non-joiner following the rules for German ligatures. "
                u"This script is based on a dictionary "
                u"that provides the fitting ligature setting "
                u"for many German words. For words that are not in the "
                u"dictionary, the script tries to guess the fitting "
                u"ligature setting. The script inserts zero\xA0width "
                u"non-joiners on all morpheme boundaries, regardless of "
                u"whether the suppressed ligature is common or available "
                u"at all in the current font face. So for the word "
                u"“Auflage” the common fl\xA0ligature is suppressed by a "
                u"zero\xA0width non-joiner. But also in the word “Aufgabe” "
                u"a zero\xA0width non-joiner is inserted between\xA0f and\xA0g "
                u"even though an fg ligature is not common. This "
                u"approach has the disadvantage that probably many "
                u"zero\xA0width non-joiners are inserted that will have no "
                u"effect because the font face does not even contain "
                u"these ligatures. But this approach has the advantage "
                u"that is works in principle for all font faces, also "
                u"if discretionary ligatures are used or if the "
                u"font face contains exotic ligatures.",
            scribus.ICON_NONE,
            scribus.BUTTON_OK,
            scribus.BUTTON_CANCEL)
    if result == 4194304:
        return
    if result != 1024:
        raise AssertionError(
            "Expected message result be 1024 or 4194304, but it isn’t.")
    my_provider = InstructionProvider()
    # Make sure that get_word_characters() has only BMP values.
    word_characters = my_provider.get_word_characters()
    for char in word_characters:
        if not is_bmp_scalar_only(char):
            raise AssertionError("get_word_characters() should return only "
                                 "BMP values, but it doesn’t.")
    try:
        scribus.setRedraw(False)
        for my_textframe in all_my_objects:
            # For each text frame in our list, we do the ligature setting
            # for the hole story (means, also for all linked text
            # frames). Result: If the user has selected various linked
            # text frames of the same story, than we do all the same
            # work various times. That’s quite inefficent, but however
            # I don’t know how to prevent this.
            ligature_setting_for_story(my_textframe, my_provider)
            # Clean the text selection for this Scribus object
            scribus.selectText(0, 0, my_textframe)
        # Show final message (if no exceptions have been raised so far)…
        if german_locale:
            show_messagebox(
                u"Ligatursatz",
                u"Der Ligatursatz ist abgeschlossen.")
        else:
            show_messagebox(
                u"Ligature setting",
                u"Ligature setting has finished.")
    finally:
        # Make sure that redraw gets enabled again.
        scribus.setRedraw(True)
        scribus.redrawAll()

if __name__ == '__main__':
    do_ligature_setting()
