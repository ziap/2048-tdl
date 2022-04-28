#include <string>

#include "webview/webview.h"

#ifndef GUI_H
#define GUI_H

void start_webview(webview::webview::sync_binding_t fn) {
    webview::webview w;

    w.bind("AIMove", fn); 

    w.set_size(1024, 768, WEBVIEW_HINT_NONE);
    w.set_size(800, 600, WEBVIEW_HINT_MIN);
    w.set_title("2048 Agent");
    w.set_html(R"(
<!DOCTYPE html>
<html>

<head>
    <meta charset="utf-8">
    <title>2048</title>
    <meta name="viewport"
        content="width=device-width, target-densitydpi=160dpi, initial-scale=1.0, maximum-scale=1, user-scalable=no, minimal-ui">
    <style>
        html,
        body {
            margin: 0;
            padding: 0;
            background: #faf8ef;
            color: #776e65;
            font-family: "Clear Sans", "Helvetica Neue", Arial, sans-serif;
            font-size: 18px;
        }

        body {
            margin: 80px 0;
        }

        .heading:after {
            content: "";
            display: block;
            clear: both;
        }

        h1.title {
            font-size: 80px;
            font-weight: bold;
            margin: 0;
            display: block;
            float: left;
        }

        @-webkit-keyframes move-up {
            0% {
                top: 25px;
                opacity: 1;
            }

            100% {
                top: -50px;
                opacity: 0;
            }
        }

        @-moz-keyframes move-up {
            0% {
                top: 25px;
                opacity: 1;
            }

            100% {
                top: -50px;
                opacity: 0;
            }
        }

        @keyframes move-up {
            0% {
                top: 25px;
                opacity: 1;
            }

            100% {
                top: -50px;
                opacity: 0;
            }
        }

        .scores-container {
            float: right;
            text-align: right;
        }

        .score-container,
        .best-container {
            position: relative;
            display: inline-block;
            background: #bbada0;
            padding: 15px 25px;
            font-size: 25px;
            height: 25px;
            line-height: 47px;
            font-weight: bold;
            border-radius: 3px;
            color: white;
            margin-top: 8px;
            text-align: center;
        }

        .score-container:after,
        .best-container:after {
            position: absolute;
            width: 100%;
            top: 10px;
            left: 0;
            text-transform: uppercase;
            font-size: 13px;
            line-height: 13px;
            text-align: center;
            color: #eee4da;
        }

        .score-container .score-addition,
        .best-container .score-addition {
            position: absolute;
            right: 30px;
            color: red;
            font-size: 25px;
            line-height: 25px;
            font-weight: bold;
            color: rgba(119, 110, 101, 0.9);
            z-index: 100;
            -webkit-animation: move-up 600ms ease-in;
            -moz-animation: move-up 600ms ease-in;
            animation: move-up 600ms ease-in;
            -webkit-animation-fill-mode: both;
            -moz-animation-fill-mode: both;
            animation-fill-mode: both;
        }

        .score-container:after {
            content: "Score";
        }

        .best-container:after {
            content: "Best";
        }

        p {
            margin-top: 0;
            margin-bottom: 10px;
            line-height: 1.65;
        }

        a {
            color: #776e65;
            font-weight: bold;
            text-decoration: underline;
            cursor: pointer;
        }

        strong.important {
            text-transform: uppercase;
        }

        hr {
            border: none;
            border-bottom: 1px solid #d8d4d0;
            margin-top: 20px;
            margin-bottom: 30px;
        }

        .container {
            width: 500px;
            margin: 0 auto;
        }

        @-webkit-keyframes fade-in {
            0% {
                opacity: 0;
            }

            100% {
                opacity: 1;
            }
        }

        @-moz-keyframes fade-in {
            0% {
                opacity: 0;
            }

            100% {
                opacity: 1;
            }
        }

        @keyframes fade-in {
            0% {
                opacity: 0;
            }

            100% {
                opacity: 1;
            }
        }

        .game-container {
            margin-top: 40px;
            position: relative;
            padding: 15px;
            cursor: default;
            -webkit-touch-callout: none;
            -ms-touch-callout: none;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            -ms-touch-action: none;
            touch-action: none;
            background: #bbada0;
            border-radius: 6px;
            width: 500px;
            height: 500px;
            -webkit-box-sizing: border-box;
            -moz-box-sizing: border-box;
            box-sizing: border-box;
        }

        .game-container .game-message {
            display: none;
            position: absolute;
            top: 0;
            right: 0;
            bottom: 0;
            left: 0;
            background: rgba(238, 228, 218, 0.5);
            z-index: 100;
            text-align: center;
            -webkit-animation: fade-in 800ms ease 1200ms;
            -moz-animation: fade-in 800ms ease 1200ms;
            animation: fade-in 800ms ease 1200ms;
            -webkit-animation-fill-mode: both;
            -moz-animation-fill-mode: both;
            animation-fill-mode: both;
        }

        .game-container .game-message p {
            font-size: 60px;
            font-weight: bold;
            height: 60px;
            line-height: 60px;
            margin-top: 222px;
        }

        .game-container .game-message .lower {
            display: block;
            margin-top: 59px;
        }

        .game-container .game-message a {
            display: inline-block;
            background: #8f7a66;
            border-radius: 3px;
            padding: 0 20px;
            text-decoration: none;
            color: #f9f6f2;
            height: 40px;
            line-height: 42px;
            margin-left: 9px;
        }

        .game-container .game-message a.keep-playing-button {
            display: none;
        }

        .game-container .game-message.game-won {
            background: rgba(237, 194, 46, 0.5);
            color: #f9f6f2;
        }

        .game-container .game-message.game-won a.keep-playing-button {
            display: inline-block;
        }

        .game-container .game-message.game-won,
        .game-container .game-message.game-over {
            display: block;
        }

        .grid-container {
            position: absolute;
            z-index: 1;
        }

        .grid-row {
            margin-bottom: 15px;
        }

        .grid-row:last-child {
            margin-bottom: 0;
        }

        .grid-row:after {
            content: "";
            display: block;
            clear: both;
        }

        .grid-cell {
            width: 106.25px;
            height: 106.25px;
            margin-right: 15px;
            float: left;
            border-radius: 3px;
            background: rgba(238, 228, 218, 0.35);
        }

        .grid-cell:last-child {
            margin-right: 0;
        }

        .tile-container {
            position: absolute;
            z-index: 2;
        }

        .tile,
        .tile .tile-inner {
            width: 107px;
            height: 107px;
            line-height: 107px;
        }

        .tile.tile-position-1-1 {
            -webkit-transform: translate(0px, 0px);
            -moz-transform: translate(0px, 0px);
            -ms-transform: translate(0px, 0px);
            transform: translate(0px, 0px);
        }

        .tile.tile-position-1-2 {
            -webkit-transform: translate(0px, 121px);
            -moz-transform: translate(0px, 121px);
            -ms-transform: translate(0px, 121px);
            transform: translate(0px, 121px);
        }

        .tile.tile-position-1-3 {
            -webkit-transform: translate(0px, 242px);
            -moz-transform: translate(0px, 242px);
            -ms-transform: translate(0px, 242px);
            transform: translate(0px, 242px);
        }

        .tile.tile-position-1-4 {
            -webkit-transform: translate(0px, 363px);
            -moz-transform: translate(0px, 363px);
            -ms-transform: translate(0px, 363px);
            transform: translate(0px, 363px);
        }

        .tile.tile-position-2-1 {
            -webkit-transform: translate(121px, 0px);
            -moz-transform: translate(121px, 0px);
            -ms-transform: translate(121px, 0px);
            transform: translate(121px, 0px);
        }

        .tile.tile-position-2-2 {
            -webkit-transform: translate(121px, 121px);
            -moz-transform: translate(121px, 121px);
            -ms-transform: translate(121px, 121px);
            transform: translate(121px, 121px);
        }

        .tile.tile-position-2-3 {
            -webkit-transform: translate(121px, 242px);
            -moz-transform: translate(121px, 242px);
            -ms-transform: translate(121px, 242px);
            transform: translate(121px, 242px);
        }

        .tile.tile-position-2-4 {
            -webkit-transform: translate(121px, 363px);
            -moz-transform: translate(121px, 363px);
            -ms-transform: translate(121px, 363px);
            transform: translate(121px, 363px);
        }

        .tile.tile-position-3-1 {
            -webkit-transform: translate(242px, 0px);
            -moz-transform: translate(242px, 0px);
            -ms-transform: translate(242px, 0px);
            transform: translate(242px, 0px);
        }

        .tile.tile-position-3-2 {
            -webkit-transform: translate(242px, 121px);
            -moz-transform: translate(242px, 121px);
            -ms-transform: translate(242px, 121px);
            transform: translate(242px, 121px);
        }

        .tile.tile-position-3-3 {
            -webkit-transform: translate(242px, 242px);
            -moz-transform: translate(242px, 242px);
            -ms-transform: translate(242px, 242px);
            transform: translate(242px, 242px);
        }

        .tile.tile-position-3-4 {
            -webkit-transform: translate(242px, 363px);
            -moz-transform: translate(242px, 363px);
            -ms-transform: translate(242px, 363px);
            transform: translate(242px, 363px);
        }

        .tile.tile-position-4-1 {
            -webkit-transform: translate(363px, 0px);
            -moz-transform: translate(363px, 0px);
            -ms-transform: translate(363px, 0px);
            transform: translate(363px, 0px);
        }

        .tile.tile-position-4-2 {
            -webkit-transform: translate(363px, 121px);
            -moz-transform: translate(363px, 121px);
            -ms-transform: translate(363px, 121px);
            transform: translate(363px, 121px);
        }

        .tile.tile-position-4-3 {
            -webkit-transform: translate(363px, 242px);
            -moz-transform: translate(363px, 242px);
            -ms-transform: translate(363px, 242px);
            transform: translate(363px, 242px);
        }

        .tile.tile-position-4-4 {
            -webkit-transform: translate(363px, 363px);
            -moz-transform: translate(363px, 363px);
            -ms-transform: translate(363px, 363px);
            transform: translate(363px, 363px);
        }

        .tile {
            position: absolute;
            -webkit-transition: 100ms ease-in-out;
            -moz-transition: 100ms ease-in-out;
            transition: 100ms ease-in-out;
            -webkit-transition-property: -webkit-transform;
            -moz-transition-property: -moz-transform;
            transition-property: transform;
        }

        .tile .tile-inner {
            border-radius: 3px;
            background: #eee4da;
            text-align: center;
            font-weight: bold;
            z-index: 10;
            font-size: 55px;
        }

        .tile.tile-2 .tile-inner {
            background: #eee4da;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0), inset 0 0 0 1px rgba(255, 255, 255, 0);
        }

        .tile.tile-4 .tile-inner {
            background: #ede0c8;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0), inset 0 0 0 1px rgba(255, 255, 255, 0);
        }

        .tile.tile-8 .tile-inner {
            color: #f9f6f2;
            background: #f2b179;
        }

        .tile.tile-16 .tile-inner {
            color: #f9f6f2;
            background: #f59563;
        }

        .tile.tile-32 .tile-inner {
            color: #f9f6f2;
            background: #f67c5f;
        }

        .tile.tile-64 .tile-inner {
            color: #f9f6f2;
            background: #f65e3b;
        }

        .tile.tile-128 .tile-inner {
            color: #f9f6f2;
            background: #edcf72;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0.2381), inset 0 0 0 1px rgba(255, 255, 255, 0.14286);
            font-size: 45px;
        }

        @media screen and (max-width: 520px) {
            .tile.tile-128 .tile-inner {
                font-size: 25px;
            }
        }

        .tile.tile-256 .tile-inner {
            color: #f9f6f2;
            background: #edcc61;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0.31746), inset 0 0 0 1px rgba(255, 255, 255, 0.19048);
            font-size: 45px;
        }

        @media screen and (max-width: 520px) {
            .tile.tile-256 .tile-inner {
                font-size: 25px;
            }
        }

        .tile.tile-512 .tile-inner {
            color: #f9f6f2;
            background: #edc850;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0.39683), inset 0 0 0 1px rgba(255, 255, 255, 0.2381);
            font-size: 45px;
        }

        @media screen and (max-width: 520px) {
            .tile.tile-512 .tile-inner {
                font-size: 25px;
            }
        }

        .tile.tile-1024 .tile-inner {
            color: #f9f6f2;
            background: #edc53f;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0.47619), inset 0 0 0 1px rgba(255, 255, 255, 0.28571);
            font-size: 35px;
        }

        @media screen and (max-width: 520px) {
            .tile.tile-1024 .tile-inner {
                font-size: 15px;
            }
        }

        .tile.tile-2048 .tile-inner {
            color: #f9f6f2;
            background: #edc22e;
            box-shadow: 0 0 30px 10px rgba(243, 215, 116, 0.55556), inset 0 0 0 1px rgba(255, 255, 255, 0.33333);
            font-size: 35px;
        }

        @media screen and (max-width: 520px) {
            .tile.tile-2048 .tile-inner {
                font-size: 15px;
            }
        }

        .tile.tile-super .tile-inner {
            color: #f9f6f2;
            background: #3c3a32;
            font-size: 30px;
        }

        @media screen and (max-width: 520px) {
            .tile.tile-super .tile-inner {
                font-size: 10px;
            }
        }

        @-webkit-keyframes appear {
            0% {
                opacity: 0;
                -webkit-transform: scale(0);
                -moz-transform: scale(0);
                -ms-transform: scale(0);
                transform: scale(0);
            }

            100% {
                opacity: 1;
                -webkit-transform: scale(1);
                -moz-transform: scale(1);
                -ms-transform: scale(1);
                transform: scale(1);
            }
        }

        @-moz-keyframes appear {
            0% {
                opacity: 0;
                -webkit-transform: scale(0);
                -moz-transform: scale(0);
                -ms-transform: scale(0);
                transform: scale(0);
            }

            100% {
                opacity: 1;
                -webkit-transform: scale(1);
                -moz-transform: scale(1);
                -ms-transform: scale(1);
                transform: scale(1);
            }
        }

        @keyframes appear {
            0% {
                opacity: 0;
                -webkit-transform: scale(0);
                -moz-transform: scale(0);
                -ms-transform: scale(0);
                transform: scale(0);
            }

            100% {
                opacity: 1;
                -webkit-transform: scale(1);
                -moz-transform: scale(1);
                -ms-transform: scale(1);
                transform: scale(1);
            }
        }

        .tile-new .tile-inner {
            -webkit-animation: appear 200ms ease 100ms;
            -moz-animation: appear 200ms ease 100ms;
            animation: appear 200ms ease 100ms;
            -webkit-animation-fill-mode: backwards;
            -moz-animation-fill-mode: backwards;
            animation-fill-mode: backwards;
        }

        @-webkit-keyframes pop {
            0% {
                -webkit-transform: scale(0);
                -moz-transform: scale(0);
                -ms-transform: scale(0);
                transform: scale(0);
            }

            50% {
                -webkit-transform: scale(1.2);
                -moz-transform: scale(1.2);
                -ms-transform: scale(1.2);
                transform: scale(1.2);
            }

            100% {
                -webkit-transform: scale(1);
                -moz-transform: scale(1);
                -ms-transform: scale(1);
                transform: scale(1);
            }
        }

        @-moz-keyframes pop {
            0% {
                -webkit-transform: scale(0);
                -moz-transform: scale(0);
                -ms-transform: scale(0);
                transform: scale(0);
            }

            50% {
                -webkit-transform: scale(1.2);
                -moz-transform: scale(1.2);
                -ms-transform: scale(1.2);
                transform: scale(1.2);
            }

            100% {
                -webkit-transform: scale(1);
                -moz-transform: scale(1);
                -ms-transform: scale(1);
                transform: scale(1);
            }
        }

        @keyframes pop {
            0% {
                -webkit-transform: scale(0);
                -moz-transform: scale(0);
                -ms-transform: scale(0);
                transform: scale(0);
            }

            50% {
                -webkit-transform: scale(1.2);
                -moz-transform: scale(1.2);
                -ms-transform: scale(1.2);
                transform: scale(1.2);
            }

            100% {
                -webkit-transform: scale(1);
                -moz-transform: scale(1);
                -ms-transform: scale(1);
                transform: scale(1);
            }
        }

        .tile-merged .tile-inner {
            z-index: 20;
            -webkit-animation: pop 200ms ease 100ms;
            -moz-animation: pop 200ms ease 100ms;
            animation: pop 200ms ease 100ms;
            -webkit-animation-fill-mode: backwards;
            -moz-animation-fill-mode: backwards;
            animation-fill-mode: backwards;
        }

        .above-game:after {
            content: "";
            display: block;
            clear: both;
        }

        .game-intro {
            float: left;
            line-height: 42px;
            margin-bottom: 0;
        }

        .restart-button, .toggle-ai {
            display: inline-block;
            background: #8f7a66;
            border-radius: 3px;
            padding: 0 20px;
            text-decoration: none;
            color: #f9f6f2;
            height: 40px;
            line-height: 42px;
            display: block;
            text-align: center;
            float: right;
        }

        .restart-button {
            display: none;
        }

        .game-explanation {
            margin-top: 50px;
        }

        @media screen and (max-width: 520px) {

            html,
            body {
                font-size: 15px;
            }

            body {
                margin: 20px 0;
                padding: 0 20px;
            }

            h1.title {
                font-size: 27px;
                margin-top: 15px;
            }

            .container {
                width: 280px;
                margin: 0 auto;
            }

            .score-container,
            .best-container {
                margin-top: 0;
                padding: 15px 10px;
                min-width: 40px;
            }

            .heading {
                margin-bottom: 10px;
            }

            .game-intro {
                width: 55%;
                display: block;
                box-sizing: border-box;
                line-height: 1.65;
            }

            .restart-button {
                width: 42%;
                padding: 0;
                display: block;
                box-sizing: border-box;
                margin-top: 2px;
            }

            .game-container {
                margin-top: 17px;
                position: relative;
                padding: 10px;
                cursor: default;
                -webkit-touch-callout: none;
                -ms-touch-callout: none;
                -webkit-user-select: none;
                -moz-user-select: none;
                -ms-user-select: none;
                -ms-touch-action: none;
                touch-action: none;
                background: #bbada0;
                border-radius: 6px;
                width: 280px;
                height: 280px;
                -webkit-box-sizing: border-box;
                -moz-box-sizing: border-box;
                box-sizing: border-box;
            }

            .game-container .game-message {
                display: none;
                position: absolute;
                top: 0;
                right: 0;
                bottom: 0;
                left: 0;
                background: rgba(238, 228, 218, 0.5);
                z-index: 100;
                text-align: center;
                -webkit-animation: fade-in 800ms ease 1200ms;
                -moz-animation: fade-in 800ms ease 1200ms;
                animation: fade-in 800ms ease 1200ms;
                -webkit-animation-fill-mode: both;
                -moz-animation-fill-mode: both;
                animation-fill-mode: both;
            }

            .game-container .game-message p {
                font-size: 60px;
                font-weight: bold;
                height: 60px;
                line-height: 60px;
                margin-top: 222px;
            }

            .game-container .game-message .lower {
                display: block;
                margin-top: 59px;
            }

            .game-container .game-message a {
                display: inline-block;
                background: #8f7a66;
                border-radius: 3px;
                padding: 0 20px;
                text-decoration: none;
                color: #f9f6f2;
                height: 40px;
                line-height: 42px;
                margin-left: 9px;
            }

            .game-container .game-message a.keep-playing-button {
                display: none;
            }

            .game-container .game-message.game-won {
                background: rgba(237, 194, 46, 0.5);
                color: #f9f6f2;
            }

            .game-container .game-message.game-won a.keep-playing-button {
                display: inline-block;
            }

            .game-container .game-message.game-won,
            .game-container .game-message.game-over {
                display: block;
            }

            .grid-container {
                position: absolute;
                z-index: 1;
            }

            .grid-row {
                margin-bottom: 10px;
            }

            .grid-row:last-child {
                margin-bottom: 0;
            }

            .grid-row:after {
                content: "";
                display: block;
                clear: both;
            }

            .grid-cell {
                width: 57.5px;
                height: 57.5px;
                margin-right: 10px;
                float: left;
                border-radius: 3px;
                background: rgba(238, 228, 218, 0.35);
            }

            .grid-cell:last-child {
                margin-right: 0;
            }

            .tile-container {
                position: absolute;
                z-index: 2;
            }

            .tile,
            .tile .tile-inner {
                width: 58px;
                height: 58px;
                line-height: 58px;
            }

            .tile.tile-position-1-1 {
                -webkit-transform: translate(0px, 0px);
                -moz-transform: translate(0px, 0px);
                -ms-transform: translate(0px, 0px);
                transform: translate(0px, 0px);
            }

            .tile.tile-position-1-2 {
                -webkit-transform: translate(0px, 67px);
                -moz-transform: translate(0px, 67px);
                -ms-transform: translate(0px, 67px);
                transform: translate(0px, 67px);
            }

            .tile.tile-position-1-3 {
                -webkit-transform: translate(0px, 135px);
                -moz-transform: translate(0px, 135px);
                -ms-transform: translate(0px, 135px);
                transform: translate(0px, 135px);
            }

            .tile.tile-position-1-4 {
                -webkit-transform: translate(0px, 202px);
                -moz-transform: translate(0px, 202px);
                -ms-transform: translate(0px, 202px);
                transform: translate(0px, 202px);
            }

            .tile.tile-position-2-1 {
                -webkit-transform: translate(67px, 0px);
                -moz-transform: translate(67px, 0px);
                -ms-transform: translate(67px, 0px);
                transform: translate(67px, 0px);
            }

            .tile.tile-position-2-2 {
                -webkit-transform: translate(67px, 67px);
                -moz-transform: translate(67px, 67px);
                -ms-transform: translate(67px, 67px);
                transform: translate(67px, 67px);
            }

            .tile.tile-position-2-3 {
                -webkit-transform: translate(67px, 135px);
                -moz-transform: translate(67px, 135px);
                -ms-transform: translate(67px, 135px);
                transform: translate(67px, 135px);
            }

            .tile.tile-position-2-4 {
                -webkit-transform: translate(67px, 202px);
                -moz-transform: translate(67px, 202px);
                -ms-transform: translate(67px, 202px);
                transform: translate(67px, 202px);
            }

            .tile.tile-position-3-1 {
                -webkit-transform: translate(135px, 0px);
                -moz-transform: translate(135px, 0px);
                -ms-transform: translate(135px, 0px);
                transform: translate(135px, 0px);
            }

            .tile.tile-position-3-2 {
                -webkit-transform: translate(135px, 67px);
                -moz-transform: translate(135px, 67px);
                -ms-transform: translate(135px, 67px);
                transform: translate(135px, 67px);
            }

            .tile.tile-position-3-3 {
                -webkit-transform: translate(135px, 135px);
                -moz-transform: translate(135px, 135px);
                -ms-transform: translate(135px, 135px);
                transform: translate(135px, 135px);
            }

            .tile.tile-position-3-4 {
                -webkit-transform: translate(135px, 202px);
                -moz-transform: translate(135px, 202px);
                -ms-transform: translate(135px, 202px);
                transform: translate(135px, 202px);
            }

            .tile.tile-position-4-1 {
                -webkit-transform: translate(202px, 0px);
                -moz-transform: translate(202px, 0px);
                -ms-transform: translate(202px, 0px);
                transform: translate(202px, 0px);
            }

            .tile.tile-position-4-2 {
                -webkit-transform: translate(202px, 67px);
                -moz-transform: translate(202px, 67px);
                -ms-transform: translate(202px, 67px);
                transform: translate(202px, 67px);
            }

            .tile.tile-position-4-3 {
                -webkit-transform: translate(202px, 135px);
                -moz-transform: translate(202px, 135px);
                -ms-transform: translate(202px, 135px);
                transform: translate(202px, 135px);
            }

            .tile.tile-position-4-4 {
                -webkit-transform: translate(202px, 202px);
                -moz-transform: translate(202px, 202px);
                -ms-transform: translate(202px, 202px);
                transform: translate(202px, 202px);
            }

            .tile .tile-inner {
                font-size: 35px;
            }

            .game-message p {
                font-size: 30px !important;
                height: 30px !important;
                line-height: 30px !important;
                margin-top: 90px !important;
            }

            .game-message .lower {
                margin-top: 30px !important;
            }
        }
    </style>
</head>

<body>
    <div class="container">
        <div class="heading">
            <h1 class="title">2048</h1>
            <div class="scores-container">
                <div class="score-container">0</div>
                <div class="best-container">0</div>
            </div>
        </div>

        <div class="above-game">
            <p class="game-intro">Join the numbers and get to the <strong>2048 tile!</strong></p>
            <a class="restart-button"></a>
            <a class="toggle-ai">Start AI</a>
        </div>

        <div class="game-container">
            <div class="game-message">
                <p></p>
                <div class="lower">
                    <a class="keep-playing-button">Keep going</a>
                    <a class="retry-button">Try again</a>
                </div>
            </div>

            <div class="grid-container">
                <div class="grid-row">
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                </div>
                <div class="grid-row">
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                </div>
                <div class="grid-row">
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                </div>
                <div class="grid-row">
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                    <div class="grid-cell"></div>
                </div>
            </div>

            <div class="tile-container">

            </div>
        </div>

        <p class="game-explanation">
            <strong class="important">How to play:</strong> Use your <strong>arrow keys</strong> to move the tiles. When
            two tiles with the same number touch, they <strong>merge into one!</strong>
        </p>
        <hr>
        <p>
            <strong class="important">Note:</strong> This site is the official version of 2048. You can play it on your
            phone via <a href="http://git.io/2048">http://git.io/2048.</a> All other apps or sites are derivatives or
            fakes, and should be used with caution.
        </p>
        <hr>
        <p>
            Created by <a href="http://gabrielecirulli.com" target="_blank">Gabriele Cirulli.</a> Based on <a
                href="https://itunes.apple.com/us/app/1024!/id823499224" target="_blank">1024 by Veewo Studio</a> and
            conceptually similar to <a href="http://asherv.com/threes/" target="_blank">Threes by Asher Vollmer.</a>
        </p>
    </div>

    <script>
        function KeyboardInputManager() {
            this.events = {};

            if (window.navigator.msPointerEnabled) {
                //Internet Explorer 10 style
                this.eventTouchstart = "MSPointerDown";
                this.eventTouchmove = "MSPointerMove";
                this.eventTouchend = "MSPointerUp";
            } else {
                this.eventTouchstart = "touchstart";
                this.eventTouchmove = "touchmove";
                this.eventTouchend = "touchend";
            }

            this.listen();
        }

        KeyboardInputManager.prototype.on = function (event, callback) {
            if (!this.events[event]) {
                this.events[event] = [];
            }
            this.events[event].push(callback);
        };

        KeyboardInputManager.prototype.emit = function (event, data) {
            var callbacks = this.events[event];
            if (callbacks) {
                callbacks.forEach(function (callback) {
                    callback(data);
                });
            }
        };

        KeyboardInputManager.prototype.listen = function () {
            var self = this;

            var map = {
                38: 0, // Up
                39: 1, // Right
                40: 2, // Down
                37: 3, // Left
                75: 0, // Vim up
                76: 1, // Vim right
                74: 2, // Vim down
                72: 3, // Vim left
                87: 0, // W
                68: 1, // D
                83: 2, // S
                65: 3  // A
            };

            // Respond to direction keys
            document.addEventListener("keydown", function (event) {
                var modifiers = event.altKey || event.ctrlKey || event.metaKey ||
                    event.shiftKey;
                var mapped = map[event.which];

                if (!modifiers) {
                    if (mapped !== undefined) {
                        event.preventDefault();
                        self.emit("move", mapped);
                    }
                }

                // R key restarts the game
                if (!modifiers && event.which === 82) {
                    self.restart.call(self, event);
                }
            });

            // Respond to button presses
            this.bindButtonPress(".retry-button", this.restart);
            this.bindButtonPress(".restart-button", this.restart);
            this.bindButtonPress(".keep-playing-button", this.keepPlaying);

            // Respond to swipe events
            var touchStartClientX, touchStartClientY;
            var gameContainer = document.getElementsByClassName("game-container")[0];

            gameContainer.addEventListener(this.eventTouchstart, function (event) {
                if ((!window.navigator.msPointerEnabled && event.touches.length > 1) ||
                    event.targetTouches.length > 1) {
                    return; // Ignore if touching with more than 1 finger
                }

                if (window.navigator.msPointerEnabled) {
                    touchStartClientX = event.pageX;
                    touchStartClientY = event.pageY;
                } else {
                    touchStartClientX = event.touches[0].clientX;
                    touchStartClientY = event.touches[0].clientY;
                }

                event.preventDefault();
            });

            gameContainer.addEventListener(this.eventTouchmove, function (event) {
                event.preventDefault();
            });

            gameContainer.addEventListener(this.eventTouchend, function (event) {
                if ((!window.navigator.msPointerEnabled && event.touches.length > 0) ||
                    event.targetTouches.length > 0) {
                    return; // Ignore if still touching with one or more fingers
                }

                var touchEndClientX, touchEndClientY;

                if (window.navigator.msPointerEnabled) {
                    touchEndClientX = event.pageX;
                    touchEndClientY = event.pageY;
                } else {
                    touchEndClientX = event.changedTouches[0].clientX;
                    touchEndClientY = event.changedTouches[0].clientY;
                }

                var dx = touchEndClientX - touchStartClientX;
                var absDx = Math.abs(dx);

                var dy = touchEndClientY - touchStartClientY;
                var absDy = Math.abs(dy);

                if (Math.max(absDx, absDy) > 10) {
                    // (right : left) : (down : up)
                    self.emit("move", absDx > absDy ? (dx > 0 ? 1 : 3) : (dy > 0 ? 2 : 0));
                }
            });
        };

        KeyboardInputManager.prototype.restart = function (event) {
            event.preventDefault();
            this.emit("restart");
        };

        KeyboardInputManager.prototype.keepPlaying = function (event) {
            event.preventDefault();
            this.emit("keepPlaying");
        };

        KeyboardInputManager.prototype.bindButtonPress = function (selector, fn) {
            var button = document.querySelector(selector);
            button.addEventListener("click", fn.bind(this));
            button.addEventListener(this.eventTouchend, fn.bind(this));
        };

        function HTMLActuator() {
            this.tileContainer = document.querySelector(".tile-container");
            this.scoreContainer = document.querySelector(".score-container");
            this.bestContainer = document.querySelector(".best-container");
            this.messageContainer = document.querySelector(".game-message");

            this.score = 0;
        }

        HTMLActuator.prototype.actuate = function (grid, metadata) {
            var self = this;

            window.requestAnimationFrame(function () {
                self.clearContainer(self.tileContainer);

                grid.cells.forEach(function (column) {
                    column.forEach(function (cell) {
                        if (cell) {
                            self.addTile(cell);
                        }
                    });
                });

                self.updateScore(metadata.score);
                self.updateBestScore(metadata.bestScore);

                if (metadata.terminated) {
                    if (metadata.over) {
                        self.message(false); // You lose
                    } else if (metadata.won) {
                        self.message(true); // You win!
                    }
                }

            });
        };

        // Continues the game (both restart and keep playing)
        HTMLActuator.prototype.continueGame = function () {
            this.clearMessage();
        };

        HTMLActuator.prototype.clearContainer = function (container) {
            while (container.firstChild) {
                container.removeChild(container.firstChild);
            }
        };

        HTMLActuator.prototype.addTile = function (tile) {
            var self = this;

            var wrapper = document.createElement("div");
            var inner = document.createElement("div");
            var position = tile.previousPosition || { x: tile.x, y: tile.y };
            var positionClass = this.positionClass(position);

            // We can't use classlist because it somehow glitches when replacing classes
            var classes = ["tile", "tile-" + tile.value, positionClass];

            if (tile.value > 2048) classes.push("tile-super");

            this.applyClasses(wrapper, classes);

            inner.classList.add("tile-inner");
            inner.textContent = tile.value;

            if (tile.previousPosition) {
                // Make sure that the tile gets rendered in the previous position first
                window.requestAnimationFrame(function () {
                    classes[2] = self.positionClass({ x: tile.x, y: tile.y });
                    self.applyClasses(wrapper, classes); // Update the position
                });
            } else if (tile.mergedFrom) {
                classes.push("tile-merged");
                this.applyClasses(wrapper, classes);

                // Render the tiles that merged
                tile.mergedFrom.forEach(function (merged) {
                    self.addTile(merged);
                });
            } else {
                classes.push("tile-new");
                this.applyClasses(wrapper, classes);
            }

            // Add the inner part of the tile to the wrapper
            wrapper.appendChild(inner);

            // Put the tile on the board
            this.tileContainer.appendChild(wrapper);
        };

        HTMLActuator.prototype.applyClasses = function (element, classes) {
            element.setAttribute("class", classes.join(" "));
        };

        HTMLActuator.prototype.normalizePosition = function (position) {
            return { x: position.x + 1, y: position.y + 1 };
        };

        HTMLActuator.prototype.positionClass = function (position) {
            position = this.normalizePosition(position);
            return "tile-position-" + position.x + "-" + position.y;
        };

        HTMLActuator.prototype.updateScore = function (score) {
            this.clearContainer(this.scoreContainer);

            var difference = score - this.score;
            this.score = score;

            this.scoreContainer.textContent = this.score;

            if (difference > 0) {
                var addition = document.createElement("div");
                addition.classList.add("score-addition");
                addition.textContent = "+" + difference;

                this.scoreContainer.appendChild(addition);
            }
        };

        HTMLActuator.prototype.updateBestScore = function (bestScore) {
            this.bestContainer.textContent = bestScore;
        };

        HTMLActuator.prototype.message = function (won) {
            var type = won ? "game-won" : "game-over";
            var message = won ? "You win!" : "Game over!";

            this.messageContainer.classList.add(type);
            this.messageContainer.getElementsByTagName("p")[0].textContent = message;
        };

        HTMLActuator.prototype.clearMessage = function () {
            // IE only takes one value to remove at a time.
            this.messageContainer.classList.remove("game-won");
            this.messageContainer.classList.remove("game-over");
        };

        function Grid(size, previousState) {
            this.size = size;
            this.cells = previousState ? this.fromState(previousState) : this.empty();
        }

        // Build a grid of the specified size
        Grid.prototype.empty = function () {
            var cells = [];

            for (var x = 0; x < this.size; x++) {
                var row = cells[x] = [];

                for (var y = 0; y < this.size; y++) {
                    row.push(null);
                }
            }

            return cells;
        };

        Grid.prototype.fromState = function (state) {
            var cells = [];

            for (var x = 0; x < this.size; x++) {
                var row = cells[x] = [];

                for (var y = 0; y < this.size; y++) {
                    var tile = state[x][y];
                    row.push(tile ? new Tile(tile.position, tile.value) : null);
                }
            }

            return cells;
        };

        // Find the first available random position
        Grid.prototype.randomAvailableCell = function () {
            var cells = this.availableCells();

            if (cells.length) {
                return cells[Math.floor(Math.random() * cells.length)];
            }
        };

        Grid.prototype.availableCells = function () {
            var cells = [];

            this.eachCell(function (x, y, tile) {
                if (!tile) {
                    cells.push({ x: x, y: y });
                }
            });

            return cells;
        };

        // Call callback for every cell
        Grid.prototype.eachCell = function (callback) {
            for (var x = 0; x < this.size; x++) {
                for (var y = 0; y < this.size; y++) {
                    callback(x, y, this.cells[x][y]);
                }
            }
        };

        // Check if there are any cells available
        Grid.prototype.cellsAvailable = function () {
            return !!this.availableCells().length;
        };

        // Check if the specified cell is taken
        Grid.prototype.cellAvailable = function (cell) {
            return !this.cellOccupied(cell);
        };

        Grid.prototype.cellOccupied = function (cell) {
            return !!this.cellContent(cell);
        };

        Grid.prototype.cellContent = function (cell) {
            if (this.withinBounds(cell)) {
                return this.cells[cell.x][cell.y];
            } else {
                return null;
            }
        };

        // Inserts a tile at its position
        Grid.prototype.insertTile = function (tile) {
            this.cells[tile.x][tile.y] = tile;
        };

        Grid.prototype.removeTile = function (tile) {
            this.cells[tile.x][tile.y] = null;
        };

        Grid.prototype.withinBounds = function (position) {
            return position.x >= 0 && position.x < this.size &&
                position.y >= 0 && position.y < this.size;
        };

        Grid.prototype.serialize = function () {
            var cellState = [];

            for (var x = 0; x < this.size; x++) {
                var row = cellState[x] = [];

                for (var y = 0; y < this.size; y++) {
                    row.push(this.cells[x][y] ? this.cells[x][y].serialize() : null);
                }
            }

            return {
                size: this.size,
                cells: cellState
            };
        };
        function Tile(position, value) {
            this.x = position.x;
            this.y = position.y;
            this.value = value || 2;

            this.previousPosition = null;
            this.mergedFrom = null; // Tracks tiles that merged together
        }

        Tile.prototype.savePosition = function () {
            this.previousPosition = { x: this.x, y: this.y };
        };

        Tile.prototype.updatePosition = function (position) {
            this.x = position.x;
            this.y = position.y;
        };

        Tile.prototype.serialize = function () {
            return {
                position: {
                    x: this.x,
                    y: this.y
                },
                value: this.value
            };
        };

        window.fakeStorage = {
            _data: {},

            setItem: function (id, val) {
                return this._data[id] = String(val);
            },

            getItem: function (id) {
                return this._data.hasOwnProperty(id) ? this._data[id] : undefined;
            },

            removeItem: function (id) {
                return delete this._data[id];
            },

            clear: function () {
                return this._data = {};
            }
        };

        function LocalStorageManager() {
            this.bestScoreKey = "bestScore";
            this.gameStateKey = "gameState";

            var supported = this.localStorageSupported();
            this.storage = supported ? window.localStorage : window.fakeStorage;
        }

        LocalStorageManager.prototype.localStorageSupported = function () {
            var testKey = "test";

            try {
                var storage = window.localStorage;
                storage.setItem(testKey, "1");
                storage.removeItem(testKey);
                return true;
            } catch (error) {
                return false;
            }
        };

        // Best score getters/setters
        LocalStorageManager.prototype.getBestScore = function () {
            return this.storage.getItem(this.bestScoreKey) || 0;
        };

        LocalStorageManager.prototype.setBestScore = function (score) {
            this.storage.setItem(this.bestScoreKey, score);
        };

        // Game state getters/setters and clearing
        LocalStorageManager.prototype.getGameState = function () {
            var stateJSON = this.storage.getItem(this.gameStateKey);
            return stateJSON ? JSON.parse(stateJSON) : null;
        };

        LocalStorageManager.prototype.setGameState = function (gameState) {
            this.storage.setItem(this.gameStateKey, JSON.stringify(gameState));
        };

        LocalStorageManager.prototype.clearGameState = function () {
            this.storage.removeItem(this.gameStateKey);
        };

        function GameManager(size, InputManager, Actuator, StorageManager) {
            this.size = size; // Size of the grid
            this.inputManager = new InputManager;
            this.storageManager = new StorageManager;
            this.actuator = new Actuator;

            this.startTiles = 2;

            this.inputManager.on("move", this.move.bind(this));
            this.inputManager.on("restart", this.restart.bind(this));
            this.inputManager.on("keepPlaying", this.keepPlaying.bind(this));

            this.setup();
        }

        // Restart the game
        GameManager.prototype.restart = function () {
            this.storageManager.clearGameState();
            this.actuator.continueGame(); // Clear the game won/lost message
            this.setup();
        };

        // Keep playing after winning (allows going over 2048)
        GameManager.prototype.keepPlaying = function () {
            this.keepPlaying = true;
            this.actuator.continueGame(); // Clear the game won/lost message
        };

        // Return true if the game is lost, or has won and the user hasn't kept playing
        GameManager.prototype.isGameTerminated = function () {
            return this.over || (this.won && !this.keepPlaying);
        };

        // Set up the game
        GameManager.prototype.setup = function () {
            var previousState = this.storageManager.getGameState();

            // Reload the game from a previous game if present
            if (previousState) {
                this.grid = new Grid(previousState.grid.size,
                    previousState.grid.cells); // Reload grid
                this.score = previousState.score;
                this.over = previousState.over;
                this.won = previousState.won;
                this.keepPlaying = previousState.keepPlaying;
            } else {
                this.grid = new Grid(this.size);
                this.score = 0;
                this.over = false;
                this.won = false;
                this.keepPlaying = false;

                // Add the initial tiles
                this.addStartTiles();
            }

            // Update the actuator
            this.actuate();
        };

        // Set up the initial tiles to start the game with
        GameManager.prototype.addStartTiles = function () {
            for (var i = 0; i < this.startTiles; i++) {
                this.addRandomTile();
            }
        };

        // Adds a tile in a random position
        GameManager.prototype.addRandomTile = function () {
            if (this.grid.cellsAvailable()) {
                var value = Math.random() < 0.9 ? 2 : 4;
                var tile = new Tile(this.grid.randomAvailableCell(), value);

                this.grid.insertTile(tile);
            }
        };

        // Sends the updated grid to the actuator
        GameManager.prototype.actuate = function () {
            if (this.storageManager.getBestScore() < this.score) {
                this.storageManager.setBestScore(this.score);
            }

            // Clear the state when the game is over (game over only, not win)
            if (this.over) {
                this.storageManager.clearGameState();
            } else {
                this.storageManager.setGameState(this.serialize());
            }

            this.actuator.actuate(this.grid, {
                score: this.score,
                over: this.over,
                won: this.won,
                bestScore: this.storageManager.getBestScore(),
                terminated: this.isGameTerminated()
            });

        };

        // Represent the current game as an object
        GameManager.prototype.serialize = function () {
            return {
                grid: this.grid.serialize(),
                score: this.score,
                over: this.over,
                won: this.won,
                keepPlaying: this.keepPlaying
            };
        };

        // Save all tile positions and remove merger info
        GameManager.prototype.prepareTiles = function () {
            this.grid.eachCell(function (x, y, tile) {
                if (tile) {
                    tile.mergedFrom = null;
                    tile.savePosition();
                }
            });
        };

        // Move a tile and its representation
        GameManager.prototype.moveTile = function (tile, cell) {
            this.grid.cells[tile.x][tile.y] = null;
            this.grid.cells[cell.x][cell.y] = tile;
            tile.updatePosition(cell);
        };

        // Move tiles on the grid in the specified direction
        GameManager.prototype.move = function (direction) {
            // 0: up, 1: right, 2: down, 3: left
            var self = this;

            if (this.isGameTerminated()) return; // Don't do anything if the game's over

            var cell, tile;

            var vector = this.getVector(direction);
            var traversals = this.buildTraversals(vector);
            var moved = false;

            // Save the current tile positions and remove merger information
            this.prepareTiles();

            // Traverse the grid in the right direction and move tiles
            traversals.x.forEach(function (x) {
                traversals.y.forEach(function (y) {
                    cell = { x: x, y: y };
                    tile = self.grid.cellContent(cell);

                    if (tile) {
                        var positions = self.findFarthestPosition(cell, vector);
                        var next = self.grid.cellContent(positions.next);

                        // Only one merger per row traversal?
                        if (next && next.value === tile.value && !next.mergedFrom) {
                            var merged = new Tile(positions.next, tile.value * 2);
                            merged.mergedFrom = [tile, next];

                            self.grid.insertTile(merged);
                            self.grid.removeTile(tile);

                            // Converge the two tiles' positions
                            tile.updatePosition(positions.next);

                            // Update the score
                            self.score += merged.value;

                            // The mighty 2048 tile
                            if (merged.value === 65536) self.won = true;
                        } else {
                            self.moveTile(tile, positions.farthest);
                        }

                        if (!self.positionsEqual(cell, tile)) {
                            moved = true; // The tile moved from its original cell!
                        }
                    }
                });
            });

            if (moved) {
                this.addRandomTile();

                if (!this.movesAvailable()) {
                    this.over = true; // Game over!
                }

                this.actuate();
            }
        };

        // Get the vector representing the chosen direction
        GameManager.prototype.getVector = function (direction) {
            // Vectors representing tile movement
            var map = {
                0: { x: 0, y: -1 }, // Up
                1: { x: 1, y: 0 },  // Right
                2: { x: 0, y: 1 },  // Down
                3: { x: -1, y: 0 }   // Left
            };

            return map[direction];
        };

        // Build a list of positions to traverse in the right order
        GameManager.prototype.buildTraversals = function (vector) {
            var traversals = { x: [], y: [] };

            for (var pos = 0; pos < this.size; pos++) {
                traversals.x.push(pos);
                traversals.y.push(pos);
            }

            // Always traverse from the farthest cell in the chosen direction
            if (vector.x === 1) traversals.x = traversals.x.reverse();
            if (vector.y === 1) traversals.y = traversals.y.reverse();

            return traversals;
        };

        GameManager.prototype.findFarthestPosition = function (cell, vector) {
            var previous;

            // Progress towards the vector direction until an obstacle is found
            do {
                previous = cell;
                cell = { x: previous.x + vector.x, y: previous.y + vector.y };
            } while (this.grid.withinBounds(cell) &&
                this.grid.cellAvailable(cell));

            return {
                farthest: previous,
                next: cell // Used to check if a merge is required
            };
        };

        GameManager.prototype.movesAvailable = function () {
            return this.grid.cellsAvailable() || this.tileMatchesAvailable();
        };

        // Check for available matches between tiles (more expensive check)
        GameManager.prototype.tileMatchesAvailable = function () {
            var self = this;

            var tile;

            for (var x = 0; x < this.size; x++) {
                for (var y = 0; y < this.size; y++) {
                    tile = this.grid.cellContent({ x: x, y: y });

                    if (tile) {
                        for (var direction = 0; direction < 4; direction++) {
                            var vector = self.getVector(direction);
                            var cell = { x: x + vector.x, y: y + vector.y };

                            var other = self.grid.cellContent(cell);

                            if (other && other.value === tile.value) {
                                return true; // These two tiles can be merged
                            }
                        }
                    }
                }
            }

            return false;
        };

        GameManager.prototype.positionsEqual = function (first, second) {
            return first.x === second.x && first.y === second.y;
        };


        // Wait till the browser is ready to render the game (avoids glitches)
        window.requestAnimationFrame(function () {
            var game = new GameManager(4, KeyboardInputManager, HTMLActuator, LocalStorageManager);

            function currentState() {
                var result = new Uint16Array(4);
                for (var i = 0; i < 4; ++i) {
                    for (var j = 0; j < 4; ++j) {
                        var tile = game.grid.cells[j][i];
                        if (tile) result[i] = result[i] | ((Math.log2(tile.value) & 0xf) << (12 - 4 * j));
                    }
                }
                return result;
            }

            var working = 0;

            function step() {
                if (!working) return;
                var state = currentState();
                
                AIMove(...state).then(result => {
                    game.move(result);
                    if (game.over) working = false;
                    else step();
                })
            }

            document.getElementsByClassName("toggle-ai")[0].addEventListener("click", function() {
                
                working = !working;
                if (working) step();
            });
        });
    </script>
</body>

</html>
    )");
    w.run();
};

#endif