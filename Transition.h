//
// Created by chrisxo on 2/14/24.
//

#ifndef REMOVE_COMMENTS_TRANSITION_H
#define REMOVE_COMMENTS_TRANSITION_H

#include "Structures.h"

void Transition(const char ch, State& state, StringData* out) {
    // our transition function, where curr is the current State™
    switch (state) {
        case State::CODE:
            if (ch == '/') { // comment watch
                state = State::SLASH;
            } else if (ch == '\n') { // newline, add to currline and such
                out->str.push_back(ch);
                out->debugLine = ++out->currentLine;
            } else if (ch == '*') { // looks for */ that is before /*
                state = State::RISK;
                out->str.push_back(ch);
            } else if (ch == '\"' || ch == '\'') { // string detector
                state = State::STRING;
                out->openingString = ch;
                out->str.push_back(ch);
            } else { // regular alphas
                out->str.push_back(ch);
            }
            break;

        case State::STRING:
            if (ch == out->openingString) { // string closed
                state = State::CODE;
                out->str.push_back(ch);
            } else if (ch == '\n') {
                state = State::ERROR;
            } else {
                out->str.push_back(ch);
            }
            break;

        case State::SLASH:
            if (ch == '/') {
                state = State::LINE_COMMENT;
                out->str.push_back(' '); // accounting for previous slash
                out->str.push_back(' '); // this slash
            } else if (ch == '*') {
                state = State::START_BLOCK;
                out->str.push_back(' '); // accounting for previous slash
                out->str.push_back(' '); // this asterisk
            } else {
                state = State::CODE;
                out->str.push_back('/'); // add the slash for the previous buffer read
                out->str.push_back(ch);  // add the char at buffer

                if (ch == '\n')
                    out->debugLine = ++out->currentLine;
            }
            break;

        case State::LINE_COMMENT:
            if (ch == '\n') {
                state = State::CODE;
                out->str.push_back(ch);
                out->debugLine = ++out->currentLine;
            } else {
                out->str.push_back(' ');
            }
            break;

        case State::START_BLOCK:
            if (ch == '*') {
                state = State::END_BLOCK;
                out->str.push_back(' ');
            } else if (ch == '\n') {
                out->str.push_back(ch);
                ++out->currentLine; // do not update debugLine until we get out of C-style
            } else {
                out->str.push_back(' ');
            }
            break;

        case State::END_BLOCK:
            if (ch == '/') {
                state = State::CODE;
                out->str.push_back(' ');
                out->debugLine = out->currentLine;
            } else if (ch == '*') {
                state = State::END_BLOCK;
                out->str.push_back(' ');
            } else if (ch == '\n') {
                state = State::START_BLOCK;
                out->str.push_back(ch);
                out->currentLine++;
            } else {
                state = State::START_BLOCK;
                out->str.push_back(' ');
            }
            break;

        case State::RISK:
            if (ch == '/') {
                state = State::ERROR;
            } else if (ch == '\n') {
                state = State::CODE;
                out->str.push_back(ch);
                out->debugLine = ++out->currentLine;
            } else if (ch == '*') {
                state = State::ERROR;
                out->str.push_back(ch);
            } else {
                state = State::CODE;
                out->str.push_back(ch);
            }
            break;

        case State::ERROR: // error is met, we just skip every buffer read
            break;
    }
}

#endif //REMOVE_COMMENTS_TRANSITION_H
