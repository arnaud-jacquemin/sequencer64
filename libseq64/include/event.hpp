#ifndef SEQ64_EVENT_HPP
#define SEQ64_EVENT_HPP

/*
 *  This file is part of seq24/sequencer64.
 *
 *  seq24 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  seq24 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with seq24; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          event.hpp
 *
 *  This module declares/defines the event class for operating with
 *  MIDI events.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2015-10-28
 * \license       GNU GPLv2 or above
 *
 *  This module also declares/defines the various constants, status-byte
 *  values, or data values for MIDI events.
 *
 */

/**
 *  Defines the number of data bytes in MIDI status data.
 */

#define MIDI_DATA_BYTE_COUNT           2

namespace seq64
{

/**
 *  This highest bit of the status byte is always 1.
 */

const unsigned char  EVENT_STATUS_BIT       = 0x80;

/**
 *  The following MIDI events are channel messages.  The comments represent
 *  the one or two data-bytes.
 */

const unsigned char  EVENT_NOTE_OFF         = 0x80;     // 0kkkkkkk 0vvvvvvv
const unsigned char  EVENT_NOTE_ON          = 0x90;     // 0kkkkkkk 0vvvvvvv
const unsigned char  EVENT_AFTERTOUCH       = 0xA0;     // 0kkkkkkk 0vvvvvvv
const unsigned char  EVENT_CONTROL_CHANGE   = 0xB0;     // 0ccccccc 0vvvvvvv
const unsigned char  EVENT_PROGRAM_CHANGE   = 0xC0;     // 0ppppppp
const unsigned char  EVENT_CHANNEL_PRESSURE = 0xD0;     // 0vvvvvvv
const unsigned char  EVENT_PITCH_WHEEL      = 0xE0;     // 0lllllll 0mmmmmmm

/**
 *  The following MIDI events have no channel.
 */

const unsigned char  EVENT_CLEAR_CHAN_MASK  = 0xF0;
const unsigned char  EVENT_MIDI_QUAR_FRAME  = 0xF1;     // not used
const unsigned char  EVENT_MIDI_SONG_POS    = 0xF2;
const unsigned char  EVENT_MIDI_SONG_SELECT = 0xF3;     // not used
const unsigned char  EVENT_MIDI_TUNE_SELECT = 0xF6;     // not used
const unsigned char  EVENT_MIDI_CLOCK       = 0xF8;
const unsigned char  EVENT_MIDI_START       = 0xFA;
const unsigned char  EVENT_MIDI_CONTINUE    = 0xFB;
const unsigned char  EVENT_MIDI_STOP        = 0xFC;
const unsigned char  EVENT_MIDI_ACTIVE_SENS = 0xFE;     // not used
const unsigned char  EVENT_MIDI_RESET       = 0xFF;     // not used

/**
 *  A MIDI System Exclusive (SYSEX) message starts with F0, followed
 *  by the manufacturer ID (how many? bytes), a number of data bytes, and
 *  ended by an F7.
 */

const unsigned char  EVENT_SYSEX            = 0xF0;
const unsigned char  EVENT_SYSEX_END        = 0xF7;

/**
 *  Provides events for management of MIDI events.
 *
 *  A MIDI event consists of 3 bytes:
 *
 *      -#  Status byte, 1sssnnn, where the sss bits specify the type of
 *          message, and the nnnn bits denote the channel number.
 *          The status byte always starts with 0.
 *      -#  The first data byte, 0xxxxxxx, where the data byte always
 *          start with 0, and the xxxxxxx values range from 0 to 127.
 *      -#  The second data byte, 0xxxxxxx.
 *
 *  This class may have too many member functions.
 */

class event
{

    friend class sequence;

private:

    /*
     *  Provides the MIDI timestamp in ticks.
     */

    unsigned long m_timestamp;

    /**
     *  This is status byte without the channel.  The channel will be
     *  appended on the MIDI bus.  The high nibble = type of event; The
     *  low nibble = channel.  Bit 7 is present in all status bytes.
     */

    unsigned char m_status;

    /**
     *  The two bytes of data for the MIDI event.  Remember that the
     *  most-significant bit of a data byte is always 0.
     */

    unsigned char m_data[MIDI_DATA_BYTE_COUNT];

    /**
     *  Points to the data buffer for SYSEX messages.
     *
     *  This really ought to be a Boost or STD scoped pointer.
     */

    unsigned char * m_sysex;

    /**
     *  Gives the size of the SYSEX message.
     */

    long m_size;

    /**
     *  This event is used to link Note Ons and Offs together.
     */

    event * m_linked;

    /**
     *  Indicates that a link has been made.  This item is used [via
     *  the get_link() and link() accessors] in the sequence class.
     */

    bool m_has_link;

    /**
     *  Answers the question "is this event selected in editing."
     */

    bool m_selected;

    /**
     *  Answers the question "is this event marked in processing."
     */

    bool m_marked;

    /**
     *  Answers the question "is this event being painted."
     */

    bool m_painted;

public:

    event ();
    ~event ();

    /*
     * Operator overload, the only one needed for sorting events in a list
     * or a map.
     */

    bool operator < (const event & rhsevent) const;

    /**
     * \setter m_timestamp
     */

    void set_timestamp (unsigned long time)
    {
        m_timestamp = time;
    }

    /**
     * \getter m_timestamp
     */

    long get_timestamp () const
    {
        return m_timestamp;
    }

    /**
     * \getter m_status
     */

    unsigned char status () const
    {
        return m_status;
    }

    /**
     *  Static test for channel messages/statuses.
     *
     * \param msg
     *      The channel status or message byte to be tested.
     *
     * \return
     *      Returns true if the byte represents a MIDI channel message.
     */

    static bool is_channel_msg (unsigned char msg)
    {
        return
        (
            msg == EVENT_NOTE_ON        || msg == EVENT_NOTE_OFF ||
            msg == EVENT_AFTERTOUCH     || msg == EVENT_CONTROL_CHANGE ||
            msg == EVENT_PROGRAM_CHANGE || msg == EVENT_CHANNEL_PRESSURE ||
            msg == EVENT_PITCH_WHEEL
        );
    }

    /**
     *  Static test for channel messages that have only one data byte.
     *  The rest have two.
     *
     * \param msg
     *      The channel status or message byte to be tested.
     *
     * \return
     *      Returns true if the byte represents a MIDI channel message that
     *      has only one data byte.  However, if this function returns false,
     *      it might not be a channel message at all, so be careful.
     */

    static bool is_one_byte_msg (unsigned char msg)
    {
        return msg == EVENT_PROGRAM_CHANGE || msg == EVENT_CHANNEL_PRESSURE;
    }

    /**
     *  Static test for channel messages that have two data bytes.
     *
     * \param msg
     *      The channel status or message byte to be tested.
     *
     * \return
     *      Returns true if the byte represents a MIDI channel message that
     *      has two data bytes.  However, if this function returns false,
     *      it might not be a channel message at all, so be careful.
     */

    static bool is_two_byte_msg (unsigned char msg)
    {
        return
        (
            msg == EVENT_NOTE_ON        || msg == EVENT_NOTE_OFF ||
            msg == EVENT_AFTERTOUCH     || msg == EVENT_CONTROL_CHANGE ||
            msg == EVENT_PITCH_WHEEL
        );
    }

    /**
     *  Static test for channel messages that are either not control-change
     *  messages, or are and match the given controller value.
     *
     * \note
     *      The old logic was the first line, but can be simplified to the
     *      second line; the third line shows the abstract representation.
     *      Also made sure of this using a couple truth tables.
     *
     *  \verbatim
            (m != EVENT_CONTROL_CHANGE) || (m == EVENT_CONTROL_CHANGE && d == cc)
            (m != EVENT_CONTROL_CHANGE) || (d == cc)
            a || (! a && b)  =>  a || b
     *  \endverbatim
     *
     * \param msg
     *      The channel status or message byte to be tested.
     *
     * \param cc
     *      The desired cc value, which the datum must match, if the message is
     *      a control-change message.
     *
     * \param datum
     *      The current datum, to be compared to cc, if the message is a
     *      control-change message.
     *
     * \return
     *      Returns true if the message is not a control-change, or if it is
     *      and the cc and datum parameters match.
     */

    static bool is_desired_cc_or_not_cc
    (
        unsigned char msg, unsigned char cc, unsigned char datum
    )
    {
        return (msg != EVENT_CONTROL_CHANGE) || (datum == cc);
    }

    /**
     *  Calculates the value of the current timestamp modulo the given
     *  parameter.
     *
     * \param a_mod
     *      The value to mod the timestamp against.
     *
     * \return
     *      Returns a value ranging from 0 to a_mod-1.
     */

    void mod_timestamp (unsigned long a_mod)
    {
        m_timestamp %= a_mod;
    }

    void set_status (char status);      // a bit long to inline

    /**
     * \getter m_status
     */

    unsigned char get_status () const
    {
        return m_status;
    }

    void set_data (char d1);
    void set_data (char d1, char d2);
    void get_data (unsigned char & d0, unsigned char & d1);
    void increment_data1 ();
    void decrement_data1 ();
    void increment_data2 ();
    void decrement_data2 ();
    void start_sysex ();
    bool append_sysex (unsigned char * data, long size);

    /**
     * \getter m_sysex
     */

    unsigned char * get_sysex () const
    {
        return m_sysex;
    }

    /**
     * \setter m_size
     */

    void set_size (long a_size)
    {
        m_size = a_size;
    }

    /**
     * \getter m_size
     */

    long get_size () const
    {
        return m_size;
    }

    /**
     *  Sets m_has_link and sets m_link to the provided event pointer.
     */

    void link (event * a_event)
    {
        m_has_link = true;
        m_linked = a_event;
    }

    /**
     * \getter m_linked
     */

    event * get_linked () const
    {
        return m_linked;
    }

    /**
     * \getter m_has_link
     */

    bool is_linked () const
    {
        return m_has_link;
    }

    /**
     * \setter m_has_link
     */

    void clear_link ()
    {
        m_has_link = false;
    }

    /**
     * \setter m_painted
     */

    void paint ()
    {
        m_painted = true;
    }

    /**
     * \setter m_painted
     */

    void unpaint ()
    {
        m_painted = false;
    }

    /**
     * \getter m_painted
     */

    bool is_painted () const
    {
        return m_painted;
    }

    /**
     * \setter m_marked
     */

    void mark ()
    {
        m_marked = true;
    }

    /**
     * \setter m_marked
     */

    void unmark ()
    {
        m_marked = false;
    }

    /**
     * \getter m_marked
     */

    bool is_marked () const
    {
        return m_marked;
    }

    /**
     * \setter m_selected
     */

    void select ()
    {
        m_selected = true;
    }

    /**
     * \setter m_selected
     */

    void unselect ()
    {
        m_selected = false;
    }

    /**
     * \getter m_selected
     */

    bool is_selected () const
    {
        return m_selected;
    }

    void make_clock ();                // set status to MIDI clock

    /**
     * \getter m_data[]
     */

    unsigned char data (int index) const    /* index not checked, for speed */
    {
        return m_data[index];
    }

    /**
     *  Assuming m_data[] holds a note, get the note number, which is in the
     *  first data byte, m_data[0].
     */

    unsigned char get_note () const
    {
        return m_data[0];
    }

    /**
     *  Sets the note number, clearing off the most-significant-bit and
     *  assigning it to the first data byte, m_data[0].
     */

    void set_note (char a_note)
    {
        m_data[0] = a_note & 0x7F;
    }

    /**
     * \getter m_data[1], the note velocity.
     */

    unsigned char get_note_velocity () const
    {
        return m_data[1];
    }

    /**
     *  Sets the note velocity, with is held in the second data byte,
     *  m_data[1].
     */

    void set_note_velocity (int a_vel)
    {
        m_data[1] = a_vel & 0x7F;
    }

    /**
     *  Returns true if m_status is EVENT_NOTE_ON.
     */

    bool is_note_on () const
    {
        return m_status == EVENT_NOTE_ON;
    }

    /**
     *  Returns true if m_status is EVENT_NOTE_OFF.
     */

    bool is_note_off () const
    {
        return m_status == EVENT_NOTE_OFF;
    }

    void print ();

    /**
     *  This function is used in sorting MIDI status events (e.g. note
     *  on/off, aftertouch, control change, etc.)  The sort order is not
     *  determined by the actual status values.
     */

    int get_rank () const;

};

}           // namespace seq64

#endif      // SEQ64_EVENT_HPP

/*
 * event.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

