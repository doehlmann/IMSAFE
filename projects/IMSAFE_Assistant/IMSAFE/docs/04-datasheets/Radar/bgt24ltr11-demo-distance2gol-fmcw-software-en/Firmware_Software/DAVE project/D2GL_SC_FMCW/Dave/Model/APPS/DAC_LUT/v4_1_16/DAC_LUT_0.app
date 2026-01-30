<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="DAC_LUT" URI="http://resources/4.1.16/app/DAC_LUT/0" description="Supports advance waveform generation using look up table(LUT)." mode="NOTSHARABLE" version="4.1.16" minDaveVersion="4.0.0" instanceLabel="DAC_LUT" appLabel="" containingProxySignal="true">
  <properties singleton="true" provideInit="true"/>
  <virtualSignals name="analog_ch_output" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output" hwSignal="out" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="analog_coupled_ch_output" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output_coupled" hwSignal="out" hwResource="//@hwResources.2" required="false" visible="true"/>
  <virtualSignals name="trigger" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_trigger" hwSignal="trig" hwResource="//@hwResources.0" visible="true">
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../CCU4_SLICE_CONFIG/v4_0_18/CCU4_SLICE_CONFIG_2.app#//@connections.1"/>
  </virtualSignals>
  <virtualSignals name="coupled_ch_trigger" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_trigger_coupled" hwSignal="trig" hwResource="//@hwResources.2" required="false" visible="true"/>
  <virtualSignals name="event_ch_sr" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_sr" hwSignal="sr" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="event_coupled_ch_sr" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_sr_coupled" hwSignal="sr" hwResource="//@hwResources.2" required="false" visible="true"/>
  <virtualSignals name="pad_signal" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad" hwSignal="pad" hwResource="//@hwResources.1" required="false"/>
  <virtualSignals name="coupled_ch_pad_signal" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad_coupled" hwSignal="pad" hwResource="//@hwResources.3" required="false"/>
  <virtualSignals name="dma_event" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dma_ch_interrupt" hwSignal="dma_int" hwResource="//@hwResources.5"/>
  <virtualSignals name="destination_trigger" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dst_trigger_dlr" hwSignal="in" hwResource="//@hwResources.6"/>
  <virtualSignals name="sr_ch" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dac_nvic_signal" hwSignal="signal_in" hwResource="//@hwResources.4" required="false" visible="true"/>
  <virtualSignals name="dlr_out" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dst_trigger_dlr_tx_out" hwSignal="out" hwResource="//@hwResources.6"/>
  <virtualSignals name="destination_request" URI="http://resources/4.1.16/app/DAC_LUT/0/vs_dma_ch_dst_req_tx" hwSignal="dest_req" hwResource="//@hwResources.5"/>
  <virtualSignals name="Pin_signal" URI="http://resources/4.1.16/app/DAC_LUT/0/__pin_vs_io_pad" hwSignal="pin" hwResource="//@hwResources.7"/>
  <virtualSignals name="Coupled CH Pin_signal" URI="http://resources/4.1.16/app/DAC_LUT/0/__pin_vs_io_pad_coupled" hwSignal="pin" hwResource="//@hwResources.8"/>
  <requiredApps URI="http://resources/4.1.16/app/DAC_LUT/0/appres_clock_0" requiredAppName="CLOCK_XMC4" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../CLOCK_XMC4/v4_0_22/CLOCK_XMC4_0.app#/"/>
  </requiredApps>
  <requiredApps URI="http://resources/4.1.16/app/DAC_LUT/0/appres_cpu" requiredAppName="CPU_CTRL_XMC4" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../CPU_CTRL_XMC4/v4_0_14/CPU_CTRL_XMC4_0.app#/"/>
  </requiredApps>
  <requiredApps URI="http://resources/4.1.16/app/DAC_LUT/0/appres_global_dma" requiredAppName="GLOBAL_DMA" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../GLOBAL_DMA/v4_0_8/GLOBAL_DMA_1.app#/"/>
  </requiredApps>
  <requiredApps URI="http://resources/4.1.16/app/DAC_LUT/0/appres_analog_io" requiredAppName="ANALOG_IO" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../ANALOG_IO/v4_0_10/ANALOG_IO_0.app#/"/>
  </requiredApps>
  <requiredApps URI="http://resources/4.1.16/app/DAC_LUT/0/appres_coupled_ch_analog_io" requiredAppName="ANALOG_IO" required="false" requiringMode="SHARABLE"/>
  <hwResources name="DAC Channel" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dac_slice" resourceGroupUri="peripheral/dac/*/slice/*" mResGrpUri="peripheral/dac/*/slice/*">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/dac/dac_0.dd#//@provided.3"/>
  </hwResources>
  <hwResources name="Pin" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dac_analogpin" resourceGroupUri="" required="false" solverVariable="true" mResGrpUri="port/p/sv0/pad/*">
    <solverVarMap index="2">
      <value variableName="sv0" solverValue="14"/>
    </solverVarMap>
    <solverVarMap index="2">
      <value variableName="sv0" solverValue="14"/>
    </solverVarMap>
  </hwResources>
  <hwResources name="DAC Coupled Channel" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dac_coupled_slice" resourceGroupUri="" required="false" mResGrpUri="peripheral/dac/*/slice/*"/>
  <hwResources name="Coupled CH Pin" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dac_coupled_analogpin" resourceGroupUri="" required="false" solverVariable="true" mResGrpUri="port/p/sv1/pad/*">
    <solverVarMap index="2">
      <value variableName="sv1" solverValue=""/>
    </solverVarMap>
    <solverVarMap index="2">
      <value variableName="sv1" solverValue=""/>
    </solverVarMap>
  </hwResources>
  <hwResources name="DAC NVIC Node" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dac_nvic_node" resourceGroupUri="" required="false" mResGrpUri="peripheral/cpu/0/nvic/interrupt/*"/>
  <hwResources name="DMA channel" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dma_channel" resourceGroupUri="peripheral/sv2/0/sv3/*" solverVariable="true" mResGrpUri="peripheral/sv2/0/sv3/*">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/gpdma0/gpdma0_0.dd#/1/@provided.0"/>
    <solverVarMap index="1">
      <value variableName="sv2" solverValue="dma8"/>
    </solverVarMap>
    <solverVarMap index="3">
      <value variableName="sv3" solverValue="ch_adv"/>
    </solverVarMap>
    <solverVarMap index="1">
      <value variableName="sv2" solverValue="dma8"/>
    </solverVarMap>
    <solverVarMap index="3">
      <value variableName="sv3" solverValue="ch_adv"/>
    </solverVarMap>
    <solverVarMap index="1">
      <value variableName="sv2" solverValue="dma8"/>
    </solverVarMap>
    <solverVarMap index="3">
      <value variableName="sv3" solverValue="ch_adv"/>
    </solverVarMap>
  </hwResources>
  <hwResources name="DLR channel" URI="http://resources/4.1.16/app/DAC_LUT/0/hwres_dma_dlr" resourceGroupUri="peripheral/dlr/0/ch/*" mResGrpUri="peripheral/dlr/0/ch/*">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/dlr/dlr_0.dd#//@provided.12"/>
  </hwResources>
  <hwResources name="Pin" URI="http://resources/4.1.16/app/DAC_LUT/0/__pin_hwres_dac_analogpin" resourceGroupUri="" required="false" mResGrpUri="devicepackage/0/*"/>
  <hwResources name="Coupled CH Pin" URI="http://resources/4.1.16/app/DAC_LUT/0/__pin_hwres_dac_coupled_analogpin" resourceGroupUri="" required="false" mResGrpUri="devicepackage/0/*"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad" systemDefined="true" sourceSignal="analog_ch_output" targetSignal="pad_signal" required="false" srcVirtualSignal="//@virtualSignals.0" targetVirtualSignal="//@virtualSignals.6"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output/http://resources/4.1.16/app/DAC_LUT/0/vsra_analog_io_pin" systemDefined="true" sourceSignal="analog_ch_output" targetSignal="pin" srcVirtualSignal="//@virtualSignals.0" proxyTargetVirtualSignalUri="http://resources/4.0.10/app/ANALOG_IO/0/vs_analog_io_pad_pin" containingProxySignal="true">
    <downwardMapList xsi:type="ResourceModel:VirtualSignal" href="../../ANALOG_IO/v4_0_10/ANALOG_IO_0.app#//@virtualSignals.0"/>
    <targetVirtualSignal href="../../ANALOG_IO/v4_0_10/ANALOG_IO_0.app#//@virtualSignals.0"/>
  </connections>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output_coupled/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad_coupled" systemDefined="true" sourceSignal="analog_coupled_ch_output" targetSignal="coupled_ch_pad_signal" required="false" srcVirtualSignal="//@virtualSignals.1" targetVirtualSignal="//@virtualSignals.7"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output_coupled/http://resources/4.1.16/app/DAC_LUT/0/vsra_coupled_ch_analog_io_pin" systemDefined="true" sourceSignal="analog_coupled_ch_output" required="false" srcVirtualSignal="//@virtualSignals.1" containingProxySignal="true"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_output_coupled/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad_coupled" systemDefined="true" sourceSignal="analog_coupled_ch_output" targetSignal="coupled_ch_pad_signal" required="false" srcVirtualSignal="//@virtualSignals.1" targetVirtualSignal="//@virtualSignals.7"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_sr/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_nvic_signal" systemDefined="true" sourceSignal="event_ch_sr" targetSignal="sr_ch" required="false" srcVirtualSignal="//@virtualSignals.4" targetVirtualSignal="//@virtualSignals.10"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dma_ch_interrupt/http://resources/4.1.16/app/DAC_LUT/0/vsra_gpdma_interrupt_tx" systemDefined="true" sourceSignal="dma_event" targetSignal="Interrupt signal" srcVirtualSignal="//@virtualSignals.8" proxyTargetVirtualSignalUri="http://resources/4.0.8/app/GLOBAL_DMA/0/interrupt_dma" containingProxySignal="true">
    <downwardMapList xsi:type="ResourceModel:VirtualSignal" href="../../GLOBAL_DMA/v4_0_8/GLOBAL_DMA_1.app#//@virtualSignals.0"/>
    <targetVirtualSignal href="../../GLOBAL_DMA/v4_0_8/GLOBAL_DMA_1.app#//@virtualSignals.0"/>
  </connections>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dst_trigger_dlr_tx_out/http://resources/4.1.16/app/DAC_LUT/0/vs_dma_ch_dst_req_tx" systemDefined="true" sourceSignal="dlr_out" targetSignal="destination_request" srcVirtualSignal="//@virtualSignals.11" targetVirtualSignal="//@virtualSignals.12"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_dac_sr/http://resources/4.1.16/app/DAC_LUT/0/vs_dst_trigger_dlr" systemDefined="true" sourceSignal="event_ch_sr" targetSignal="destination_trigger" srcVirtualSignal="//@virtualSignals.4" targetVirtualSignal="//@virtualSignals.9"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad/http://resources/4.1.16/app/DAC_LUT/0/__pin_vs_io_pad" systemDefined="true" sourceSignal="pad_signal" targetSignal="Pin_signal" srcVirtualSignal="//@virtualSignals.6" targetVirtualSignal="//@virtualSignals.13"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/__pin_vs_io_pad/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad" systemDefined="true" sourceSignal="Pin_signal" targetSignal="pad_signal" srcVirtualSignal="//@virtualSignals.13" targetVirtualSignal="//@virtualSignals.6"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad_coupled/http://resources/4.1.16/app/DAC_LUT/0/__pin_vs_io_pad_coupled" systemDefined="true" sourceSignal="coupled_ch_pad_signal" targetSignal="Coupled CH Pin_signal" srcVirtualSignal="//@virtualSignals.7" targetVirtualSignal="//@virtualSignals.14"/>
  <connections URI="http://resources/4.1.16/app/DAC_LUT/0/http://resources/4.1.16/app/DAC_LUT/0/__pin_vs_io_pad_coupled/http://resources/4.1.16/app/DAC_LUT/0/vs_io_pad_coupled" systemDefined="true" sourceSignal="Coupled CH Pin_signal" targetSignal="coupled_ch_pad_signal" srcVirtualSignal="//@virtualSignals.14" targetVirtualSignal="//@virtualSignals.7"/>
</ResourceModel:App>
