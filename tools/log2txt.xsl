<?xml version="1.0" encoding="UTF-8" ?>

<!-- New document created with EditiX at Sun Jan 30 22:17:28 EST 2011 -->

<xsl:stylesheet version="2.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:fn="http://www.w3.org/2005/xpath-functions"
	xmlns:xdt="http://www.w3.org/2005/xpath-datatypes"
	xmlns:err="http://www.w3.org/2005/xqt-errors"
	exclude-result-prefixes="xs xdt err fn">

	<xsl:output method="text"/>
	
	<xsl:template match="/">		
<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="Log">
<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="LogMessage">
[<xsl:value-of select="@level"/>](<xsl:value-of select="@time"/>) :: <xsl:value-of select="."/>
	</xsl:template>	

</xsl:stylesheet>
