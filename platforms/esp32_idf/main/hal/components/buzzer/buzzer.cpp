/**
 * @file buzzer.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
// https://github.com/jasonacox/TinyRTTTL
#include "buzzer.h"
#include "../../hal_config.h"
#include "../utils/daemon_controller/daemon_controller.h"
#include "esp32-hal.h"
#include <Arduino.h>
#include <mooncake_log.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define OCTAVE_OFFSET 0

static const int _notes[] PROGMEM = {
    0,        NOTE_C4,  NOTE_CS4, NOTE_D4,  NOTE_DS4, NOTE_E4,  NOTE_F4,  NOTE_FS4, NOTE_G4,  NOTE_GS4,
    NOTE_A4,  NOTE_AS4, NOTE_B4,  NOTE_C5,  NOTE_CS5, NOTE_D5,  NOTE_DS5, NOTE_E5,  NOTE_F5,  NOTE_FS5,
    NOTE_G5,  NOTE_GS5, NOTE_A5,  NOTE_AS5, NOTE_B5,  NOTE_C6,  NOTE_CS6, NOTE_D6,  NOTE_DS6, NOTE_E6,
    NOTE_F6,  NOTE_FS6, NOTE_G6,  NOTE_GS6, NOTE_A6,  NOTE_AS6, NOTE_B6,  NOTE_C7,  NOTE_CS7, NOTE_D7,
    NOTE_DS7, NOTE_E7,  NOTE_F7,  NOTE_FS7, NOTE_G7,  NOTE_GS7, NOTE_A7,  NOTE_AS7, NOTE_B7};

class DaemonControllerRtttl_t : public DaemonController_t {
public:
    std::string rtttlMusic;
};
static DaemonControllerRtttl_t _daemon_controller;

static void _daemon_rtttl_music_player(void* param)
{
    mclog::tagInfo("rtttl", "start player");
    mclog::tagInfo("rtttl", "music:\n > {}", _daemon_controller.rtttlMusic);

    const char* rtttlMusic = _daemon_controller.rtttlMusic.c_str();

    // Absolutely no error checking in here
    byte default_dur = 4;
    byte default_oct = 6;
    int bpm = 63;
    int num;
    long wholenote;
    long duration;
    byte note;
    byte scale;

    // format: d=N,o=N,b=NNN:
    // find the start (skip name, etc)
    while (*rtttlMusic != ':')
        rtttlMusic++; // ignore name
    rtttlMusic++;     // skip ':'

    // get default duration
    if (*rtttlMusic == 'd') {
        rtttlMusic++;
        rtttlMusic++; // skip "d="
        num = 0;
        while (isdigit(*rtttlMusic)) {
            num = (num * 10) + (*rtttlMusic++ - '0');
        }
        if (num > 0)
            default_dur = num;
        rtttlMusic++; // skip comma
    }

    // get default octave
    if (*rtttlMusic == 'o') {
        rtttlMusic++;
        rtttlMusic++; // skip "o="
        num = *rtttlMusic++ - '0';
        if (num >= 3 && num <= 7)
            default_oct = num;
        rtttlMusic++; // skip comma
    }

    // get BPM
    if (*rtttlMusic == 'b') {
        rtttlMusic++;
        rtttlMusic++; // skip "b="
        num = 0;
        while (isdigit(*rtttlMusic)) {
            num = (num * 10) + (*rtttlMusic++ - '0');
        }
        bpm = num;
        rtttlMusic++; // skip colon
    }

    // BPM usually expresses the number of quarter notes per minute
    wholenote = (60 * 1000L / bpm) * 4; // this is the time for whole note (in milliseconds)

    // now begin note loop
    while (*rtttlMusic) {
        // first, get note duration, if available
        num = 0;
        while (isdigit(*rtttlMusic)) {
            num = (num * 10) + (*rtttlMusic++ - '0');
        }

        if (num)
            duration = wholenote / num;
        else
            duration = wholenote / default_dur; // we will need to check if we are a dotted note after

        // now get the note
        note = 0;

        switch (*rtttlMusic) {
            case 'c':
                note = 1;
                break;
            case 'd':
                note = 3;
                break;
            case 'e':
                note = 5;
                break;
            case 'f':
                note = 6;
                break;
            case 'g':
                note = 8;
                break;
            case 'a':
                note = 10;
                break;
            case 'b':
                note = 12;
                break;
            case 'p':
            default:
                note = 0;
        }
        rtttlMusic++;

        // now, get optional '#' sharp
        if (*rtttlMusic == '#') {
            note++;
            rtttlMusic++;
        }

        // now, get optional '.' dotted note
        if (*rtttlMusic == '.') {
            duration += duration / 2;
            rtttlMusic++;
        }

        // now, get scale
        if (isdigit(*rtttlMusic)) {
            scale = *rtttlMusic - '0';
            rtttlMusic++;
        } else {
            scale = default_oct;
        }

        scale += OCTAVE_OFFSET;

        if (*rtttlMusic == ',')
            rtttlMusic++; // skip comma for next note (or we may be at the end)

        // Check kill signal
        if (_daemon_controller.CheckKillSignal()) {
            mclog::tagInfo("rtttl", "get kill signal, break");
            break;
        }

        // now play the note or rest
        if (note) {
            tone(HAL_PIN_BUZZ_CTRL, _notes[(scale - 4) * 12 + note], duration);
            delay(duration);
        } else {
            delay(duration);
        }
    }

    mclog::tagInfo("rtttl", "play done");

    _daemon_controller.DestoryDaemon();

    vTaskDelete(NULL);
}

void BuzzerArduino::playRtttlMusic(const std::string& rtttlMusic)
{
    if (isPlaying()) {
        _daemon_controller.SendKillSignalAndWait();
        delay(10);
    }

    _daemon_controller.rtttlMusic = rtttlMusic;
    _daemon_controller.CreateDaemon();
    xTaskCreate(_daemon_rtttl_music_player, "rtttl", 4000, NULL, 10, NULL);
}

bool BuzzerArduino::isPlaying()
{
    return _daemon_controller.IsDaemonExist();
}

void BuzzerArduino::stop()
{
    if (isPlaying()) {
        _daemon_controller.SendKillSignal();
    } else {
        noTone(HAL_PIN_BUZZ_CTRL);
    }
}

void BuzzerArduino::beep(float frequency, std::uint32_t duration = 0xFFFFFFFF)
{
    stop();
    tone(HAL_PIN_BUZZ_CTRL, frequency, duration);
}
