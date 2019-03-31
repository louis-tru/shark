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

var fs = require('../libs/qkit/fs');
var path = require('path');
var syscall = require('../libs/qkit/syscall').syscall;
var copy_header = require('./cp-header').copy_header;
var large_file_cut = require('../libs/qkit/large_file_cut').cut;
var read_qgr_version = require('./read_version').read_qgr_version
var argv = process.argv.slice(2);
var os = argv.shift();
var name = argv.shift();
var cut = argv.shift(); // is cut
var inc = argv.shift(); // copy include files
var out_dir = argv.shift();
var framework_dir = path.resolve(`${out_dir}/${name}.framework`);
var source = __dirname + '/..';

if ( argv.length == 0 ) {
	throw new Error('Bad argument.');
}

function read_qgr_version_str() {
	var versions = read_qgr_version();
	var a = versions[0] < 10 ? '0' + versions[0] : versions[0];
	var b = versions[1] < 10 ? '0' + versions[1] : versions[1];
	var c = versions[2] < 10 ? '0' + versions[2] : versions[2];
	return `${a}.${b}.${c}`;
}

var version = read_qgr_version_str();

function read_plist_and_replace_version() {
	var placeholder_name = 'xxxxxxxxxx';
	var l = Math.max(0, placeholder_name.length - name.length + 1);
	var r_name = name + new Array(l).join(' ');
	var str = fs.readFileSync(`${__dirname}/${os}-framework.plist`).toString('hex');
	str = str.replace(new RegExp(new Buffer('11.11.11').toString('hex'), 'gm'),
										new Buffer(version).toString('hex'));
	str = str.replace(new RegExp(new Buffer(placeholder_name).toString('hex'), 'gm'), 
										new Buffer(r_name).toString('hex'));
	return new Buffer(str, 'hex');
}

fs.mkdir_p_sync(framework_dir);

// write plist
fs.writeFileSync(framework_dir + '/Info.plist', read_plist_and_replace_version());
// copy header
if (inc != 'no-inc') {
	var src = inc || source + '/qgr';
	copy_header(src, framework_dir + '/Headers');
}
// Merge dynamic library
syscall(`lipo -create ${argv.join(' ')} -output ${framework_dir}/${name}`);

if (cut === 'cut') {
	if ( fs.statSync(`${framework_dir}/${name}`).size > 1024 * 1024 * 50 ) { // > 50mb
		large_file_cut(`${framework_dir}/${name}`, 4);
		fs.rm_r(`${framework_dir}/${name}`);
	}
}

