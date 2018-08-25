package com.audio.reader;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

import javazoom.spi.mpeg.sampled.file.MpegAudioFileReader;

import java.io.File;


public class MP3Reader{
    private static MP3Reader mp3Reader = null;
    private MP3Reader(){}

    //双判断，解决单利问题
    public static MP3Reader getInstance(){
        if(mp3Reader == null){
            synchronized (MP3Reader.class) {
                if(mp3Reader == null){
                    mp3Reader = new MP3Reader();
                }
            }
        }
        return mp3Reader;
    }

    public AudioInputStream getPcmAudioInputStream(String mp3filepath) {
        Integer sampleSizeInByte = 2;
        return getPcmAudioInputStream(mp3filepath, sampleSizeInByte);
    }

    public AudioInputStream getPcmAudioInputStream(String mp3filepath, Integer sampleSizeInByte) {
        File mp3 = new File(mp3filepath);
        AudioInputStream audioInputStream = null;
        AudioFormat targetFormat = null;
        try {
            AudioInputStream in = null;

            //读取音频文件的类
            MpegAudioFileReader mp = new MpegAudioFileReader();
            in = mp.getAudioInputStream(mp3);
            AudioFormat baseFormat = in.getFormat();
            //设定输出格式为pcm格式的音频文件
            targetFormat = new AudioFormat(AudioFormat.Encoding.PCM_SIGNED, baseFormat.getSampleRate(), sampleSizeInByte * 8,
                    baseFormat.getChannels(), baseFormat.getChannels() * sampleSizeInByte, baseFormat.getSampleRate(), false);

            //输出到音频
            audioInputStream = AudioSystem.getAudioInputStream(targetFormat, in);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return audioInputStream;
    }
}