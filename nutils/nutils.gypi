{
	'targets': [
	{
		'target_name': 'nutils',
		# 'product_prefix': '',
		# 'product_ext': 'so',
		'type': '<(output_type)',
		'dependencies': [
			'nutils/minizip.gyp:minizip',
			'depe/bplus/bplus.gyp:bplus',
			'depe/node/deps/uv/uv.gyp:libuv',
			'depe/node/deps/openssl/openssl.gyp:openssl',
			'depe/node/deps/http_parser/http_parser.gyp:http_parser',
		],
		'direct_dependent_settings': {
			'include_dirs': [ '..' ],
			'mac_bundle_resources': [
			],
		},
		'include_dirs': [
			'..',
			'../depe/rapidjson/include',
			'../depe/bplus/include',
			'../depe/node/deps/zlib',
			'../depe/node/deps/zlib/contrib/minizip',
		],
		'sources': [
			'../Makefile',
			'../README.md',
			'../configure',
			'../tools/configure.js',
			'../tools/install-android-toolchain',
			'../ngui/version.h',
			'../ngui/errno.h',
			'cacert.pem',
			# src
			'env.h',
			'errno.h',
			'macros.h',
			'container.h',
			'object.h',
			'object.cc',
			'array.h',
			'array.inl',
			'codec.h',
			'error.h',
			'event.h',
			'http.h',
			'fs.h',
			'handle.h',
			'buffer.h',
			'json.h',
			'iterator.h',
			'list.h',
			'list.inl',
			'map.h',
			'map.inl',
			'string.h',
			'string.inl',
			'string.cc',
			'string-builder.h',
			'string-builder.inl',
			'string-builder.cc',
			'util.h',
			'zlib.h',
			'array.cc.inl',
			'array.cc',
			'codec.cc',
			'error.cc',
			'http.cc',
			'http-uri.cc',
			'http-helper.cc',
			'fs.cc',
			'fs-file.cc',
			'fs-sync.cc',
			'fs-async.cc',
			'fs-search.cc',
			'fs-reader.cc',
			'buffer.cc',
			'json.cc',
			'map.cc',
			'util.cc',
			'zlib.cc',
			'loop.h',
			'loop-1.h',
			'loop.cc',
			'loop-private.cc',
			'net.h',
			'net.cc',
			'uv-1.h',
			'cb.h',
			'cb.cc',
			'date.cc',
			'http-cookie.h',
			'http-cookie.cc',
			'localstorage.h',
			'localstorage.cc',
		],
		'conditions': [
			['os=="android"', {
				'conditions': [['<(android_api_level)<24', {
					'defines!': [ '_FILE_OFFSET_BITS=64' ],
				}]],
				'sources':[
					'../android/android.h',
					'../android/android.cc',
					'android-jni.h',
					'android-jni.cc',
					'android-log.h',
					'android-log.cc',
					'_android.cc',
				],
				'link_settings': {
					'libraries': [
						'-latomic', 
						'-llog', 
						'-landroid',
						'-lz',
					],
				},
			}],
			['os=="linux"', {
				'sources': [
					'_linux.cc',
				],
				'link_settings': {
					'libraries': [
						'-lz',
					]
				},
			}],
			['OS=="mac"', {
				'sources': [
					'_mac.mm',
				],
				'link_settings': {
					'libraries': [
						'$(SDKROOT)/usr/lib/libz.tbd',
					]
				},
			}],
		],
	}, 
	],
}
