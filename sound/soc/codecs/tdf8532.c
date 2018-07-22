/*
 * tdf8532.c  --  driver for NXP Semiconductors TDF8532
 *
 * Copyright (C) 2016 Intel Corp.
 * Author: Steffen Wagner <steffen.wagner@intel.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#define DEBUG

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <sound/soc.h>
#include <sound/tlv.h>
#include <sound/pcm_params.h>

struct tdf8532_priv{
	struct i2c_client* tdf8532_i2c_client;
};

struct tdf8532_priv tdf8532;

static int tdf8532_dai_startup(struct snd_pcm_substream *substream,
		struct snd_soc_dai *dai)
{
	printk(KERN_DEBUG "%s\n",__func__);
	return 0;
}

static int tdf8532_dai_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params,
		struct snd_soc_dai *dai)
{
	printk(KERN_DEBUG "%s\n",__func__);
	return 0;
}

static int tdf8532_dai_trigger(struct snd_pcm_substream *substream, int cmd,
		struct snd_soc_dai *dai)
{
	int ret = 0;
	unsigned char data[6] = {0x02, 0x00, 0x03, 0x80, 0x1A, 0x01};
	printk(KERN_DEBUG "%s: cmd = %d\n",__func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_RESUME:
		data[5]=0x01;
		break;

	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_STOP:
		data[5]=0x00;
		break;
	}
	ret = i2c_master_send(tdf8532.tdf8532_i2c_client, data, 6);
	printk(KERN_DEBUG "%s: return value of i2c master send: %d\n",__func__,ret);
	if(ret <= 0){
		return ret;
	}
	return 0;
}

static int tdf8532_dai_prepare(struct snd_pcm_substream *substream,struct snd_soc_dai *dai)
{
	printk(KERN_DEBUG "%s\n",__func__);
	return 0;
}


static const struct snd_soc_dai_ops tdf8532_dai_ops = {
		.startup  = tdf8532_dai_startup,
		.hw_params  = tdf8532_dai_hw_params,
		.trigger  = tdf8532_dai_trigger,
		.prepare  = tdf8532_dai_prepare,
};

static struct snd_soc_codec_driver soc_codec_tdf8532;

#define TDF8532_RATES SNDRV_PCM_RATE_48000
#define TDF8532_FORMATS SNDRV_PCM_FMTBIT_S16_LE

static struct snd_soc_dai_driver tdf8532_dai[] = {
		{
				.name = "tdf8532-hifi",
				.playback = {
						.stream_name = "Playback",
						.channels_min = 1,
						.channels_max = 4,
						.rates = TDF8532_RATES,
						.formats = TDF8532_FORMATS, },
						.ops = &tdf8532_dai_ops,
		}
};


static int tdf8532_i2c_probe(struct i2c_client *i2c,
		const struct i2c_device_id *id)
{

	int ret;
	tdf8532.tdf8532_i2c_client = i2c;

	printk(KERN_DEBUG "%s\n",__func__);
	ret = snd_soc_register_codec(&i2c->dev, &soc_codec_tdf8532,
			tdf8532_dai, ARRAY_SIZE(tdf8532_dai));
	if(ret!= 0)
	{
		dev_err(&i2c->dev,"%s failed!!\n",__func__);
		return ret;
	}

	return 0;

}

static int tdf8532_i2c_remove(struct i2c_client *i2c)
{
	snd_soc_unregister_codec(&i2c->dev);
	return 0;
}

static const struct i2c_device_id tdf8532_i2c_id[] = {
		{ "tdf8532", 0 },
		{ }
};

MODULE_DEVICE_TABLE(i2c, tdf8532_i2c_id);

static struct i2c_driver tdf8532_i2c_driver = {
		.driver = {
				.name = "tdf8532-codec",
				.owner = THIS_MODULE,
		},
		.probe =    tdf8532_i2c_probe,
		.remove =   tdf8532_i2c_remove,
		.id_table = tdf8532_i2c_id,
};

module_i2c_driver(tdf8532_i2c_driver);

MODULE_DESCRIPTION("ASoC NXP Semiconductors TDF8532 driver");
MODULE_AUTHOR("Steffen Wagner <steffen.wagner@intel.com>");
MODULE_LICENSE("GPL");
