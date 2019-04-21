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

import { Div, Button, CSS, Input, Text, atomPixel } from 'qgr';
import { List } from 'qgr/list';
import { Mynavpage } from './public';

var resolve = require.resolve;

function add(evt) {
	var text = evt.sender.owner.find('input').value;
	evt.sender.owner.find('list').push({ text: text });
}

function remove(evt) {
	evt.sender.owner.find('list').pop();
}

function keyenter(evt) {
	evt.sender.blur();
}

export const vx = (
	<Mynavpage title="List" source=resolve(__filename)>
		<Div width="full">
			<Input id="input" class="input" 
				value="Hello." returnType="done" onKeyEnter=keyenter />
			<Button class="long_btn" onClick=add>Add</Button>
			<Button class="long_btn" onClick=remove>Remove</Button>

			<List id="list">
				<Div margin=10 width="full">
					<Text margin=4 width="full" 
						borderBottom=`${atomPixel} #aaa`>%{$.$index + 1 + ': ' + $.text}</Text>
				</Div>
			</List>

		</Div>
	</Mynavpage>
)