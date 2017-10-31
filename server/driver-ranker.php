<?php
$exec_file = "/run/media/veracrypt1/Data/LCTF2017_BeRealDriver/code/cmake-build-release/BeRealDriver";
$root_dir = "/tmp/srvdir/";
function mkdir_if_okay($dirname)
{
    if (!file_exists($dirname)) {
        mkdir($dirname);
    }
};
function check_token($token)
{
    if (preg_match('/[^a-fA-F0-9$]/', $token)) {
        return false;
    };
    return true;
}
?>
<html>
<head>
    <title> Be a real driver! </title>
</head>
<body>
    <h1> Be a real driver! </h1>
    <h3> Welcome to LCTF 2017. Submit your token and files here, and you can get something out. </h3>
    <h3> This is <b>NOT</b> a pwn problem, so do not waste your time using <pre><code>' and 1=1</code></pre> and <pre><code>AAAAAAAAAAAAA....</code></pre></h3>
    <h3> They are COMPLETELY Useless.</h3>
    <h3> Per file max size is about 1.5M</h3>
    <?php
    // Test if anything submitted
    if (empty($_POST)) {
        ?>
        <form method="POST" enctype="multipart/form-data">
        <input type="text" name="token" id="token" value="Enter your token here"><br>
        <input type="file" name="map" id="map"><br>
        <input type="file" name="op"  id="op"><br>
        <input type="submit" value="Check"><br>
        <?php
    } else {
        echo "<hr>";
        $token = strval($_POST["token"]);
        $map = $_FILES["map"]["tmp_name"];
        $op = $_FILES["op"]["tmp_name"];
        $accept = false;
        // here check if token exists.
        $accept = $token?check_token($token):false;
        $accept = $map?$accept:false;
        $accept = $op?$accept:false;
        if (!$accept) {?>
        <h3> GIVE ME YOUR RIGHT TOKEN OR I WILL BEAT YOU THROUGH THE NETWORK LINE IMMIDATELY!!</h3>
        <?php
        } else {
            // Build a temp folder
            mkdir_if_okay($root_dir);
            $usr_dir = $root_dir . $token;//token is certainly safe so preserve it
            mkdir_if_okay($usr_dir);
            $usr_submit_time = strval(time());

            // Check if already succeed
            $usr_succeed_fname = $usr_dir . "/is_succeed";
            if (file_exists($usr_succeed_fname)) {
                $usr_succeed_time = strval(file_get_contents($usr_succeed_fname));
                $usr_succeed_dir = $usr_dir . "/" . $usr_succeed_time ;
                mkdir_if_okay($usr_succeed_dir);    // Should exists, but make sure we will not meet strange strings
                $usr_succeed_log_name = $usr_succeed_dir . "/stdout.log";
                // Try to get log
                $usr_succeed_log = file_get_contents($usr_succeed_log_name);
                if ($usr_succeed_log) {
                    echo "<h3>Seems you have got everything you want at ctime=$usr_succeed_time. Here is your log, and please contact admin if you really need.</h3>";
                    die("<pre><code>$usr_succeed_log</code></pre>");
                } else {
                    // Strange. No log is not something good.
                    unlink($usr_succeed_fname);
                }
            };

            // Seems a new submit. Create submit dir
            $usr_submit_dir = $usr_dir . "/" . "$usr_submit_time";
            mkdir_if_okay($usr_submit_dir);

            $usr_map_fname = $usr_submit_dir . "/map_offset";
            $usr_op_fname = $usr_submit_dir . "/op_offset";
            $usr_stdout = $usr_submit_dir . "/stdout.log";
            $usr_stderr = $usr_submit_dir . "/stderr.log";

            move_uploaded_file($map, $usr_map_fname);
            move_uploaded_file($op, $usr_op_fname);

            // Use proc_open to get everything
            $descriptorspec = array(
            0 => array("pipe", "r"),
            1 => array("file", $usr_stdout, "a"),
            2 => array("file", $usr_stderr, "a")
            );

            $process = proc_open("time $exec_file $token $usr_submit_time $usr_map_fname $usr_op_fname", $descriptorspec, $pipes, $usr_submit_dir);
            if (!is_resource($process)) {
                die("<h1>OJ IS DOWN! CONTACT ADMIN!</h1>");
            }

            fclose($pipes[0]);
            $return_value = proc_close($process);

            ?>
        <h3> Correct token received. Result is here (if you see nothing, you are GG): </h3><br>
            <?php
            $usr_stdout_contents = file_get_contents($usr_stdout);
            echo "<pre><code>$usr_stdout_contents</code></pre>";
            
            if ($return_value) {
                echo "<p>Ummm.....Seems GG</p>";
            } else {
                echo "<h2> 666! </h2>";
                file_put_contents($usr_succeed_fname, $usr_submit_time);
            }
        };
    };
    ?>
</body>
</html>

