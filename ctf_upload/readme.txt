题目名称：
	BeRealDriver
题目描述：
	驾照考试越来越严了，好不容易找到一个包过的。去驾校报道，教练给了我一个U盘，里面放了个文件，还放了IDA 7.0的安装包。哇，大城市的驾校果然有钱，连IDA都送。就是密码要自己搞。教练给了个神秘网址：http://showmethepassword.l-ctf.org
题目Flag：
	LCTF{We1c0m3_to_ldw_wor1d_And_is_th3_malf0rmed_fi13_1337_en0ugh_f0r_me}
难度：
	四颗星	
部署注意：
	1. 将showmethepassword.l-ctf.org绑定到IP地址__上
	2. 将showmethepassword.l-ctf.org放到cloudflare后，并联系silver获取cloudflare配置，以免被D
	3. 默认放stripped版本。34小时后如果没有队伍解出，视情况放Unstripped版本。
题目Writeup：
	http://github.com/SilverBut/LCTF2017_BeRealDriver
	（私有Repo，比赛结束后开放）
争议解决：
	1. 后台有日志，要求有争议的队伍提交token和上传文件的时间（Unix Time）即可
	2. 本地检查提交日志和上传文件，如果没有问题按照上传文件时间（Unix Time）后第一个解出该题的队伍赋同分，其他队伍分数不受影响
	3. 后台日志里没有记录的（token或上传文件），检查上传文件大小是否超出限制，token给定是否有问题
	4. 以上排查如果没有问题，视情况单独处理
