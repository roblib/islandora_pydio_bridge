<xsl:stylesheet version="1.0" 
	xmlns="http://datacite.org/schema/kernel-3"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/"
	xmlns:g="ddi:group:3_2"
				xmlns:d="ddi:datacollection:3_2"
				xmlns:dce="ddi:dcelements:3_2"
				xmlns:c="ddi:conceptualcomponent:3_2"
				xmlns:xhtml="http://www.w3.org/1999/xhtml"
				xmlns:a="ddi:archive:3_2"
				xmlns:m1="ddi:physicaldataproduct/ncube/normal:3_2"
				xmlns:ddi="ddi:instance:3_2"
				xmlns:m2="ddi:physicaldataproduct/ncube/tabular:3_2"
				xmlns:l="ddi:logicalproduct:3_2"
				xmlns:m3="ddi:physicaldataproduct/ncube/inline:3_2"
				xmlns:pd="ddi:physicaldataproduct:3_2"
				xmlns:cm="ddi:comparative:3_2"
				xmlns:s="ddi:studyunit:3_2"
				xmlns:r="ddi:reusable:3_2"
				xmlns:pi="ddi:physicalinstance:3_2"
				xmlns:ds="ddi:dataset:3_2"
				xmlns:pr="ddi:profile:3_2"
				xmlns:ru="ddi:reusable:3_2"
				xsi:schemaLocation="http://datacite.org/schema/kernel-3 http://schema.datacite.org/meta/kernel-3/metadata.xsd"
    exclude-result-prefixes="ddi">
    <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
    <xsl:template match="/">

        <oai_dc:dc>
        <xsl:for-each select="//r:Citation/r:Title">
            <dc:title>
                <xsl:value-of select="."/>
            </dc:title>
        </xsl:for-each>
        <xsl:for-each select="//r:Citation/r:InternationalIdentifier[r:ManagingAgency/text() = 'Datacite']/r:IdentifierContent">
            <dc:identifier>
                <xsl:value-of select="."/>
            </dc:identifier>
        </xsl:for-each>
		<xsl:for-each select="//r:Citation/r:Creator/r:CreatorName/r:String">
			<dc:creator>
				<xsl:value-of select="."/>
			</dc:creator>
		</xsl:for-each>
		<xsl:for-each select="//r:Citation/r:Contributor/r:ContributorName/r:String">
			<dc:contributor>
				<xsl:value-of select="."/>
			</dc:contributor>
		</xsl:for-each>
		<xsl:for-each select="//r:Citation/r:PublicationDate/r:SimpleDate">
			<dc:date>
				<xsl:value-of select="."/>
			</dc:date>
		</xsl:for-each>	
		<xsl:for-each select="//s:StudyUnit/r:Coverage/r:TopicalCoverage/r:Subject">
			<dc:subject>
				<xsl:value-of select="."/>
			</dc:subject>
		</xsl:for-each>
		<xsl:for-each select="//s:StudyUnit/r:Coverage/r:SpatialCoverage/r:Description">
			<dc:description>
				<xsl:value-of select="."/>
			</dc:description>
		</xsl:for-each>
			<xsl:for-each select="//s:StudyUnit/r:Abstract">
				<dc:description>
					<xsl:value-of select="."/>
				</dc:description>
			</xsl:for-each>
		<!--<xsl:for-each select="//codeBook:stdyInfo/codeBook:sumDscr/codeBook:geogCover">
			<dc:coverage>
				<xsl:value-of select="."/>
			</dc:coverage>
		</xsl:for-each>-->
		<xsl:for-each select="//s:StudyUnit/r:KindOfData">
			<dc:type>
				<xsl:value-of select="."/>
			</dc:type>
		</xsl:for-each>
		<xsl:for-each select="//d:DataSource/d:SourceDescription/r:Content">
			<dc:source>
				<xsl:value-of select="."/>
			</dc:source>
		</xsl:for-each>
		<xsl:for-each select="//r:Copyright/r:String">
			<dc:rights>
				<xsl:value-of select="."/>
			</dc:rights>
		</xsl:for-each>
		<!--<xsl:for-each select="//codeBook:otherStdyMat/codeBook:relMat">
			<dc:relation>
				<xsl:value-of select="."/>
			</dc:relation>
		</xsl:for-each>
		<xsl:for-each select="//codeBook:otherStdyMat/codeBook:relPubl">
			<dc:isPartOf>
				<xsl:value-of select="."/>
			</dc:isPartOf>
		</xsl:for-each>  -->
		</oai_dc:dc>
	</xsl:template>
</xsl:stylesheet>
