/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, xuewen.chu
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of xuewen.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL xuewen.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

import {
	HighlightedStatus,
	Notification, EventNoticer, GUIEvent,
	GUIHighlightedEvent, GUIKeyEvent,
	GUIClickEvent, GUITouchEvent,
	GUIMouseEvent, GUIActionEvent,
} from './event';
import * as value from './value';
import ViewController from './ctr';
import { StyleSheet } from './css';
import { Action, ActionIn, KeyframeOptions } from './_action';

const _ftr = __require__('_ftr');

export interface DOM {
	id: string;
	readonly __meta__: View;
	readonly owner: ViewController | null;
	remove(): void;
	appendTo(parentView: View): View;
	afterTo(prevView: View): View;
	style: StyleSheet;
};

/**
 * @class View
 */
export declare class View extends Notification<GUIEvent> implements DOM {
	readonly onKeyDown: EventNoticer<GUIKeyEvent>;
	readonly onKeyPress: EventNoticer<GUIKeyEvent>;
	readonly onKeyUp: EventNoticer<GUIKeyEvent>;
	readonly onKeyEnter: EventNoticer<GUIKeyEvent>;
	readonly onBack: EventNoticer<GUIClickEvent>;
	readonly onClick: EventNoticer<GUIClickEvent>;
	readonly onTouchStart: EventNoticer<GUITouchEvent>;
	readonly onTouchMove: EventNoticer<GUITouchEvent>;
	readonly onTouchEnd: EventNoticer<GUITouchEvent>;
	readonly onTouchCancel: EventNoticer<GUITouchEvent>;
	readonly onMouseOver: EventNoticer<GUIMouseEvent>;
	readonly onMouseOut: EventNoticer<GUIMouseEvent>;
	readonly onMouseLeave: EventNoticer<GUIMouseEvent>;
	readonly onMouseEnter: EventNoticer<GUIMouseEvent>;
	readonly onMouseMove: EventNoticer<GUIMouseEvent>;
	readonly onMouseDown: EventNoticer<GUIMouseEvent>;
	readonly onMouseUp: EventNoticer<GUIMouseEvent>;
	readonly onMouseWheel: EventNoticer<GUIMouseEvent>;
	readonly onFocus: EventNoticer<GUIEvent>;
	readonly onBlur: EventNoticer<GUIEvent>;
	readonly onHighlighted: EventNoticer<GUIHighlightedEvent>;
	readonly onActionKeyframe: EventNoticer<GUIActionEvent>;
	readonly onActionLoop: EventNoticer<GUIActionEvent>;
	prepend(view: View): void;
	append(view: View): void;
	appendText(str: string): void;
	before(view: View): void;
	after(view: View): void;
	remove(): void;
	removeAllChild(): void;
	focus(): boolean;
	blur(): boolean;
	layoutOffset(): value.Vec2;
	layoutOffsetFrom(parents: View): value.Vec2;
	getAction(): Action | null;
	private _setAction(action: Action | null): void;
	screenRect(): value.Rect;
	finalMatrix(): value.Mat;
	finalOpacity(): number;
	position(): value.Vec2;
	overlapTest(): boolean;
	addClass(cls: string): void;
	removeClass(): void;
	toggleClass(): void;
	firstButton(): Button | null;
	hasChild(child: View): boolean;
	readonly innerText: string;
	readonly parent: View | null;
	readonly prev: View | null;
	readonly next: View | null;
	readonly first: View | null;
	readonly last: View | null;
	x: number;
	y: number;
	scaleX: number;
	scaleY: number;
	rotateZ: number;
	skewX: number;
	skewY: number;
	opacity: number;
	visible: boolean;
	readonly finalVisible: boolean;
	readonly drawVisible: boolean;
	translate: value.Vec2;
	scale: value.Vec2;
	skew: value.Vec2;
	originX: number;
	originY: number;
	origin: value.Vec2;
	readonly matrix: value.Mat;
	readonly level: number;
	needDraw: boolean;
	receive: boolean;
	isFocus: boolean;
	readonly viewType: number;
	class: string;
	id: string; // ext
	readonly __meta__: View;
	readonly owner: ViewController | null;
	ownerAs<T extends ViewController = ViewController>(): T;
	action: Action | null;
	actionAs<T extends Action = Action>(): T;
	style: StyleSheet;
	setAction(action: ActionIn | null): void;
	hashCode(): number;
	appendTo(parentView: View): this;
	afterTo(prevView: View): this;
	transition(style: KeyframeOptions, cb?: (e: GUIActionEvent)=>void): Action;
	transition(style: KeyframeOptions, delay?: number, cb?: (e: GUIActionEvent)=>void): Action;
	show(): void;
	hide(): void;
	static readonly isViewController: boolean;
}

type Texture = any;

export declare class Sprite extends View {
	src: string;
	texture: Texture | null;
	startX: number;
	startY: number;
	width: number;
	height: number;
	start: number;
	ratioX: number;
	ratioY: number;
	ratio: value.Vec2;
	repeat: value.Repeat;
}

export interface TextFont {
	simpleLayoutWidth(str: string): number;
	textBackgroundColor: value.TextColor;
	textColor: value.TextColor;
	textSize: value.TextSize;
	textStyle: value.TextStyle;
	textFamily: value.TextFamily;
	textShadow: value.TextShadow;
	textLineHeight: value.TextLineHeight;
	textDecoration: value.TextDecoration;
}

export interface TextLayout extends TextFont {
	textOverflow: value.TextOverflow;
	textWhiteSpace: value.TextWhiteSpace;
}

export declare abstract class Layout extends View {
	readonly clientWidth: number;
	readonly clientHeight: number;
}

export declare abstract class Box extends Layout {
	width: value.Value;
	height: value.Value;
	margin: value.Value[]; //
	marginLeft: value.Value;
	marginTop: value.Value;
	marginRight: value.Value;
	marginBottom: value.Value;
	border: value.Border[]; //
	borderLeft: value.Border;
	borderTop: value.Border;
	borderRight: value.Border;
	borderBottom: value.Border;
	borderWidth: number[]; //
	borderLeftWidth: number;
	borderTopWidth: number;
	borderRightWidth: number;
	borderBottomWidth: number;
	borderColor: value.Color[];
	borderLeftColor: value.Color;
	borderTopColor: value.Color;
	borderRightColor: value.Color;
	borderBottomColor: value.Color;
	borderRadius: number[]; //
	borderRadiusLeftTop: number;
	borderRadiusRightTop: number;
	borderRadiusRightBottom: number;
	borderRadiusLeftBottom: number;
	backgroundColor: value.Color;
	background: value.Background | null;
	newline: boolean;
	clip: boolean;
	readonly finalWidth: number;
	readonly finalHeight: number;
	readonly finalMarginLeft: number;
	readonly finalMarginTop: number;
	readonly finalMarginRight: number;
	readonly finalMarginBottom: number;
}

export declare class Span extends Layout implements TextLayout {
	simpleLayoutWidth(str: string): number;
	textBackgroundColor: value.TextColor;
	textColor: value.TextColor;
	textSize: value.TextSize;
	textStyle: value.TextStyle;
	textFamily: value.TextFamily;
	textShadow: value.TextShadow;
	textLineHeight: value.TextLineHeight;
	textDecoration: value.TextDecoration;
	textOverflow: value.TextOverflow;
	textWhiteSpace: value.TextWhiteSpace;
}

export declare class TextNode extends Span {
	readonly length: number;
	value: string;
	readonly textHoriBearing: number;
	readonly textHeight: number;
}

export declare class Div extends Box {
	contentAlign: value.ContentAlign;
}

export declare class Image extends Div {
	readonly onLoad: EventNoticer<GUIEvent>;
	readonly onError: EventNoticer<GUIEvent<Error>>;
	src: string;
	readonly sourceWidth: number;
	readonly sourceHeight: number;
}

export declare class Panel extends Div {
	readonly onFocusMove: EventNoticer<GUIEvent>;
	allowLeave: boolean;
	allowEntry: boolean;
	intervalTime: number;
	enableSelect: boolean;
	readonly isActivity: boolean;
	readonly parentPanel: Panel;
}

export interface IScroll {
	readonly onScroll: EventNoticer<GUIEvent>;
	scrollTo(value: value.Vec2, duration?: number, curve?: value.Curve): void;
	terminate(): void;
	scroll: value.Vec2;
	scrollX: number;
	scrollY: number;
	readonly scrollWidth: number;
	readonly scrollHeight: number;
	scrollbar: boolean;
	resistance: number;
	bounce: boolean;
	bounceLock: boolean;
	momentum: boolean;
	lockDirection: boolean;
	catchPositionX: number;
	catchPositionY: number;
	scrollbarColor: value.Color;
	readonly hScrollbar: boolean;
	readonly vScrollbar: boolean;
	scrollbarWidth: number;
	scrollbarMargin: number;
	defaultScrollDuration: number;
	defaultScrollCurve: value.Curve;
}

export declare class Scroll extends Panel implements IScroll {
	focusMarginLeft: number;
	focusMarginRight: number;
	focusMarginTop: number;
	focusMarginBottom: number;
	focusAlignX: value.Align;
	focusAlignY: value.Align;
	enableFocusAlign: boolean;
	readonly isFixedScrollSize: boolean;
	setFixedScrollSize(size: value.Vec2): void;
	// implements IScroll
	readonly onScroll: EventNoticer<GUIEvent>;
	scrollTo(value: value.Vec2, duration?: number, curve?: value.Curve): void;
	terminate(): void;
	scroll: value.Vec2;
	scrollX: number;
	scrollY: number;
	readonly scrollWidth: number;
	readonly scrollHeight: number;
	scrollbar: boolean;
	resistance: number;
	bounce: boolean;
	bounceLock: boolean;
	momentum: boolean;
	lockDirection: boolean;
	catchPositionX: number;
	catchPositionY: number;
	scrollbarColor: value.Color;
	readonly hScrollbar: boolean;
	readonly vScrollbar: boolean;
	scrollbarWidth: number;
	scrollbarMargin: number;
	defaultScrollDuration: number;
	defaultScrollCurve: value.Curve;
}

export declare class Indep extends Div {
	alignX: value.Align;
	alignY: value.Align;
	align: value.Vec2[];
}

export interface ILimit {
	minWidth: value.Value;
	minHeight: value.Value;
	maxWidth: value.Value;
	maxHeight: value.Value;
}

export declare class Limit extends Div implements ILimit {
	// implements ILimit
	minWidth: value.Value;
	minHeight: value.Value;
	maxWidth: value.Value;
	maxHeight: value.Value;
}

export declare class LimitIndep extends Indep implements ILimit {
	// implements ILimit
	minWidth: value.Value;
	minHeight: value.Value;
	maxWidth: value.Value;
	maxHeight: value.Value;
}

export declare class Hybrid extends Box implements TextLayout {
	textAlign: value.TextAlign;
	// implements TextLayout
	simpleLayoutWidth(str: string): number;
	textBackgroundColor: value.TextColor;
	textColor: value.TextColor;
	textSize: value.TextSize;
	textStyle: value.TextStyle;
	textFamily: value.TextFamily;
	textShadow: value.TextShadow;
	textLineHeight: value.TextLineHeight;
	textDecoration: value.TextDecoration;
	textOverflow: value.TextOverflow;
	textWhiteSpace: value.TextWhiteSpace;
}

export declare class Label extends View implements TextFont {
	readonly length: number;
	value: string;
	readonly textHoriBearing: number;
	readonly textHeight: number;
	textAlign: value.TextAlign;
	// implements TextFont
	simpleLayoutWidth(str: string): number;
	textBackgroundColor: value.TextColor;
	textColor: value.TextColor;
	textSize: value.TextSize;
	textStyle: value.TextStyle;
	textFamily: value.TextFamily;
	textShadow: value.TextShadow;
	textLineHeight: value.TextLineHeight;
	textDecoration: value.TextDecoration;
}

export declare class Text extends Hybrid {
	readonly length: number;
	value: string;
	readonly textHoriBearing: number;
	readonly textHeight: number;
}

export declare class Input extends Text {
	readonly onChange: EventNoticer<GUIEvent>;
	type: value.KeyboardType;
	returnType: value.KeyboardReturnType;
	placeholder: string;
	placeholderColor: value.Color;
	security: boolean;
	textMargin: number;
}

export declare class Textarea extends Input implements IScroll {
	// implements IScroll
	readonly onScroll: EventNoticer<GUIEvent>;
	scrollTo(value: value.Vec2, duration?: number, curve?: value.Curve): void;
	terminate(): void;
	scroll: value.Vec2;
	scrollX: number;
	scrollY: number;
	readonly scrollWidth: number;
	readonly scrollHeight: number;
	scrollbar: boolean;
	resistance: number;
	bounce: boolean;
	bounceLock: boolean;
	momentum: boolean;
	lockDirection: boolean;
	catchPositionX: number;
	catchPositionY: number;
	scrollbarColor: value.Color;
	readonly hScrollbar: boolean;
	readonly vScrollbar: boolean;
	scrollbarWidth: number;
	scrollbarMargin: number;
	defaultScrollDuration: number;
	defaultScrollCurve: value.Curve;
}

export declare class Button extends Hybrid {
	findNextButton(): Button | null;
	panel(): Panel | null;
	defaultStyle: boolean;
	setHighlighted(status: HighlightedStatus): void;
	triggerHighlighted(evt: GUIHighlightedEvent): number;
}

export declare class Root extends Panel {}

export class Clip extends (_ftr.Div as typeof Div) {
	constructor() {
		super();
		this.clip = true;
	}
}

Object.assign(exports, {
	View: _ftr.View,
	Sprite: _ftr.Sprite,
	Label: _ftr.Label,
	Span: _ftr.Span,
	TextNode: _ftr.TextNode,
	Div: _ftr.Div,
	Image: _ftr.Image,
	Textarea: _ftr.Textarea,
	Panel: _ftr.Panel,
	Scroll: _ftr.Scroll,
	Indep: _ftr.Indep,
	Limit: _ftr.Limit,
	LimitIndep: _ftr.LimitIndep,
	Hybrid: _ftr.Hybrid,
	Text: _ftr.Text,
	Input: _ftr.Input,
	Button: _ftr.Button,
	Root: _ftr.Root,
});