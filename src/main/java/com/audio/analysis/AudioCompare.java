package com.audio.analysis;

import com.audio.reader.MP3Reader;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class AudioCompare{

    public static void main(String[] args){
        String exampleFileName = "src/audio/example.mp3";
        List<Float> samples = getSamples(exampleFileName);
        System.out.println(samples.size());
        samples.subList(10000, 10500).forEach(v ->{
                System.out.println(v);
        });
    }

    static List<Float> getSamples(String fileName){
        List<Float> audioSamples = new ArrayList<>();
        try{
            MP3Reader mp3Reader = MP3Reader.getInstance();
            AudioInputStream audioInputStream = mp3Reader.getPcmAudioInputStream(fileName, 2);
            AudioFormat audioFormat = audioInputStream.getFormat();
            Integer batchSeconds = 1; // 10ms
            Integer bytesPerFrame = audioFormat.getChannels() * audioFormat.getSampleSizeInBits() / 8;
            Integer numBytes = (int)(bytesPerFrame * audioFormat.getSampleRate() * batchSeconds);
            byte[] audioBytes = new byte[numBytes];
            Integer numBytesRead = 0;
            while ((numBytesRead = audioInputStream.read(audioBytes)) != -1){
                Integer numFramesRead = numBytesRead / bytesPerFrame;
                for (Integer iFrame = 0; iFrame < numFramesRead; iFrame++){
                    byte[] frame = Arrays.copyOfRange(audioBytes, iFrame*bytesPerFrame, (iFrame+1)*bytesPerFrame);
                    Float sample = audioFrameToSample(frame, audioFormat.getChannels(), audioFormat.getSampleSizeInBits());
                    audioSamples.add(sample);
                }
            }
        }catch (Exception e) {
            e.printStackTrace();
        }
        return audioSamples;
    }

    static private Float audioFrameToSample(byte[] bytesInFrame, Integer numChannel, Integer sampleSizeInBits){
        Float sample = (float)0.0;
        for (Integer iChannel = 0; iChannel < numChannel; iChannel ++){
            switch (sampleSizeInBits){
                case 8:
                    sample += bytesInFrame[iChannel] / (float)255.0;
                    break;
                case 16:
                    sample += ((bytesInFrame[iChannel*2] + bytesInFrame[iChannel*2+1] * 255) / (float)32767);
                    break;
                default:
                    sample = (float)-1.0;
            }
        }
        sample = sample / numChannel;
        return sample;
    }




}