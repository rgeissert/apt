<xsl:stylesheet
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 version="1.0">

<xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl" />

<xsl:param name="man.output.encoding" select="'UTF-8'" />
<!-- LANGUAGE -->

<xsl:template match="email">&lt;<xsl:apply-templates/>&gt;</xsl:template>

<xsl:template match="date">
	<xsl:call-template name="datetime.format">
		<xsl:with-param name="date" select="."/>
		<xsl:with-param name="format" select="'d B Y'"/>
	</xsl:call-template>
</xsl:template>

</xsl:stylesheet>
