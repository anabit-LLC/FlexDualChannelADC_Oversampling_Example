# FlexDualChannelADC_Oversampling_Example
This example Arduino sketch is meant to work with Anabit's Flex Dual Channel ADC open source reference design

Product link: https://anabit.co/products/flex-dual-channel-adc

The Flex Dual Channel ADC design uses a single eneded 14 bit 2MSPS ADC that includes a 2 channel MUX that allows you to route signals on either input channel
to the ADC using a simple digital logic level. 

This sketch deomonstrats the concept of oversampling. It makes a single ADC measurement and then makes 16 measurements that are averaged together to show the
increased resolution benefit of oversampling. With 16 equally spaced samples being averaged together you should see a resolution increase benefit of about
2 bits. By default the Flex ADC has 14 bits of resolution and this oversampling example can push that resolution to 16 bits. See the Anabit YoutTube at
the following link to learn more about oversampling: 

Please report any issue with the sketch to the Anagit forum: https://anabit.co/community/forum/analog-to-digital-converters-adcs
Example code developed by Your Anabit LLC © 2025
Licensed under the Apache License, Version 2.0.
