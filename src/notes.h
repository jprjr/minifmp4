/* boxes required for CMAF headers
 * (0) means 0 or 1 (optional but limited to 1)
 * (1) means exactly 1
 * (+) means 1 or more
 * (*) means 0 or more
 * (CM) means there's some kind of special rule
 * ftyp (1)
 * moov (1)
 *          mvhd (1)
 *          trak (+)
 *                   tkhd (1)
 *                   edts (CM)
 *                              elst (CM)
 *                   mdia (1)
 *                              mdhd (1)
 *                              hdlr (1)
 *                              elng (0)
 *                              minf (1)
 *                                         vmhd (CM)
 *                                         smhd (CM)
 *                                         sthd (CM)
 *                                         dinf (1)
 *                                                    dref (1)
 *                                         stbl (1)
 *                                                    stsd (1)
 *                                                    stts (1)
 *                                                    stsc (1)
 *                                                    stsz (1)
 *                                                    stco (1)
 *                                                    sgpd (*)
 *                   udta (0)
 *                              cprt (+)
 *                              kind (+)
 *          mvex (1)
 *                   mehd (0)
 *                   trex (+)
 *          pssh (*)
 *
 *  notes:
 *    edts and elst - edit box is not required, if it is the entry count shall be 1
 *
 * boxes required for CMAF segments
 *
 * styp (0)
 * prft (0)
 * emsg (0)
 * moof (+)
 *           mfhd (1)
 *           traf (1)
 *                    tfhd (1)
 *                    tfdt (1)
 *                    trun (1)
 *                    senc (0)
 *                    saio (*)
 *                    saiz (*)
 *                    sbgp (*)
 *                    sgpd (*)
 * mdat (+)
 *
 *
 */

